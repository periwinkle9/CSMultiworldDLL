#include "Connection.h"
#include "ConnectionManager.h"
#include <utility>
#include <exception>
#include <cstdint>
#include <sstream>
#include <format>
#include <string_view>
#include <chrono>
#include <asio/buffer.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/read.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>
#include "../RequestQueue.h"
#include "../RequestTypes.h"
#include "../Logger.h"
#include "../game_hooks.h"
#include "../uuid.h"
#include "doukutsu/flags.h"
#include "doukutsu/inventory.h"
#include "doukutsu/map.h"
#include "doukutsu/player.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using asio::ip::tcp;

Connection::Connection(tcp::socket sock, ConnectionManager& manager) :
	socket(std::move(sock)), connectionManager(manager), request(), response()
{}

void Connection::start()
{
	logger.logInfo("Connection established, starting request handler");
	asio::co_spawn(socket.get_executor(), [self = shared_from_this()]{return self->handleRequest();}, asio::detached);
}

void Connection::stop()
{
	logger.logInfo("Shutting down connection");
	socket.shutdown(tcp::socket::shutdown_both);
	socket.close();
}

asio::awaitable<void> Connection::handleRequest()
{
	try
	{
		while (socket.is_open())
		{
			// Read header
			co_await asio::async_read(socket, asio::buffer(request.header), asio::use_awaitable);
			std::uint32_t dataSize = 0;
			for (int i = 0; i < 4; ++i)
				dataSize |= (request.header[i + 1] << (8 * i));
			request.data.clear();

			// Read data
			if (dataSize > 0)
			{
				request.data.resize(dataSize);
				co_await asio::async_read(socket, asio::buffer(request.data), asio::use_awaitable);
			}

			prepareResponse();

			// Send reply
			if (!response.empty())
				co_await asio::async_write(socket, asio::buffer(response), asio::use_awaitable);
		}
	}
	catch (std::exception&)
	{
		connectionManager.stop(shared_from_this());
	}
}

static std::vector<std::int32_t> parseNumberList(const std::vector<char>& data)
{
	std::vector<std::int32_t> output;
	output.reserve(data.size() / 4);
	for (std::vector<char>::size_type i = 0; i < data.size(); i += 4)
	{
		std::int32_t value = 0;
		for (unsigned bit = 0; bit < 4 && i + bit < data.size(); ++bit)
			value |= (static_cast<unsigned char>(data[i + bit]) << (8 * bit));
		output.push_back(value);
	}
	return output;
}

static std::string getServerInfoString()
{
	constexpr int API_Version = 0;
	// Manually construct JSON string :')
	constexpr char outputFormat[] = R"!!({{
	"api_version": {},
	"platform": "{}",
	"uuid": "{}",
	"offsets": {{
		"flags": {},
		"map_flags": {},
		"arms_data": {},
		"current_hp": {},
		"max_hp": {}
	}}
}})!!";
	/*if (!uuidInitialized)
		loadUUID();*/ // Just in case
	return std::format(outputFormat, API_Version, "freeware", getUUIDString(),
		reinterpret_cast<std::uint32_t>(&csvanilla::gFlagNPC),
		reinterpret_cast<std::uint32_t>(&csvanilla::gMapping),
		reinterpret_cast<std::uint32_t>(&csvanilla::gArmsData),
		reinterpret_cast<std::uint32_t>(&csvanilla::gMC.life),
		reinterpret_cast<std::uint32_t>(&csvanilla::gMC.max_life));
}

void Connection::prepareResponse()
{
	enum RequestType: unsigned char {HANDSHAKE, EXEC_SCRIPT, GET_FLAGS, QUEUE_EVENTS, READ_MEMORY, WRITE_MEMORY, QUERY_GAME_STATE, DISCONNECT = 255};

	{
		logger.logInfo(std::format("Received request: type = {}, size = {}", static_cast<int>(request.header[0]), request.data.size()));
		std::ostringstream oss;
		oss << "Raw request data: " << std::hex;
		for (auto byte : request.data)
			oss << static_cast<unsigned>(static_cast<unsigned char>(byte)) << ' ';
		logger.logDebug(oss.str());
	}

	auto makeError = [this](std::string_view message) {
		response.clear();
		response.push_back(-1);
		for (int i = 0; i < 4; ++i)
			response.push_back(static_cast<char>((message.size() >> (i * 8)) & 0xFF));
		response.insert(response.end(), message.begin(), message.end());
		logger.logError(std::string{message});
	};
	response.clear();
	switch (request.header[0])
	{
	case HANDSHAKE:
	{
		std::string serverInfo = getServerInfoString();
		response.push_back(HANDSHAKE);
		for (int i = 0; i < 4; ++i)
			response.push_back(static_cast<char>((serverInfo.size() >> (i * 8)) & 0xFF));
		response.insert(response.end(), serverInfo.begin(), serverInfo.end());
		break;
	}
	case EXEC_SCRIPT:
		if (requestQueue != nullptr)
		{
			RequestQueue::Request event;
			event.type = RequestQueue::Request::RequestType::SCRIPT;
			event.data = std::string(request.data.begin(), request.data.end());
			
			logger.logInfo("Queueing execution of script: " + std::any_cast<std::string>(event.data));
			requestQueue->push(std::move(event));
			
			response.push_back(EXEC_SCRIPT);
			response.resize(5);
		}
		else
		{
			// Request queue not available? That's not good...
			makeError("[1] Event queue not initialized"); // Send back error status
		}
		break;
	case GET_FLAGS:
	{
		RequestTypes::FlagRequest flagRequest{};
		flagRequest.flags = parseNumberList(request.data);
		{
			std::ostringstream oss;
			oss << "Received request for flags: ";
			for (std::int32_t flag : flagRequest.flags)
				oss << flag << ' ';
			logger.logInfo(oss.str());
		}

		response.reserve(flagRequest.flags.size() + 5);
		response.push_back(GET_FLAGS);
		// Write response length
		for (int i = 0; i < 4; ++i)
			response.push_back(static_cast<char>((flagRequest.flags.size() >> (i * 8)) & 0xFF));

		// Submit request and wait for it to be fulfilled
		flagRequest.fulfilled = false;
		if (requestQueue != nullptr)
		{
			RequestQueue::Request req;
			req.type = RequestQueue::Request::RequestType::FLAGS;
			req.data = &flagRequest;
			requestQueue->push(std::move(req));

			using namespace std::chrono_literals;
			std::unique_lock<std::mutex> lock{flagRequest.mutex};
			if (flagRequest.cv.wait_for(lock, 1s, [&flagRequest]() -> bool { return flagRequest.fulfilled; }))
				response.insert(response.end(), flagRequest.result.begin(), flagRequest.result.end());
			else
				makeError("[2] Request timed out");
		}
		else
			makeError("[2] Request queue not initialized");

		break;
	}
	case QUEUE_EVENTS:
		if (requestQueue != nullptr)
		{
			std::vector<std::int32_t> eventList = parseNumberList(request.data);
			{
				std::ostringstream oss;
				oss << "Queueing execution of events: ";
				for (std::int32_t event : eventList)
					oss << event << ' ';
				logger.logInfo(oss.str());
			}

			std::vector<RequestQueue::Request> eventRequests;
			eventRequests.reserve(eventList.size());
			for (auto eventNum : eventList)
			{
				RequestQueue::Request req;
				req.type = RequestQueue::Request::RequestType::EVENTNUM;
				req.data = eventNum;
				eventRequests.push_back(req);
			}
			requestQueue->pushMultiple(eventRequests);

			response.push_back(QUEUE_EVENTS);
			response.resize(5);
		}
		else
		{
			// Request queue not available? That's not good...
			makeError("[3] Event queue not initialized"); // Send back error status
		}
		break;
	case READ_MEMORY:
		if (request.data.size() == 6)
		{
			RequestTypes::MemoryReadRequest memReadReq{};
			memReadReq.address = 0;
			for (int i = 0; i < 4; ++i)
				memReadReq.address |= (static_cast<unsigned char>(request.data[i]) << (i * 8));
			memReadReq.numBytes = static_cast<unsigned char>(request.data[4]) | (static_cast<unsigned char>(request.data[5]) << 8);
			logger.logInfo(std::format("Received request for {} bytes of memory starting at address {:#x}", memReadReq.numBytes, memReadReq.address));
			response.push_back(READ_MEMORY);
			response.push_back(request.data[4]);
			response.push_back(request.data[5]);
			response.resize(5);

			// Submit request and wait for it to be fulfilled
			memReadReq.fulfilled = false;
			if (requestQueue != nullptr)
			{
				RequestQueue::Request req;
				req.type = RequestQueue::Request::RequestType::MEMREAD;
				req.data = &memReadReq;
				requestQueue->push(std::move(req));

				using namespace std::chrono_literals;
				std::unique_lock<std::mutex> lock{memReadReq.mutex};
				if (memReadReq.cv.wait_for(lock, 1s, [&memReadReq]() -> bool { return memReadReq.fulfilled; }))
				{
					if (memReadReq.errorCode == 0)
						response.insert(response.end(), memReadReq.result.begin(), memReadReq.result.end());
					else
						makeError(std::format("[4] ReadProcessMemory failed: error code {}", memReadReq.errorCode));
				}
				else
					makeError("[4] Request timed out");
			}
			else
				makeError("[4] Request queue not initialized");
		}
		else
			makeError(std::format("[4] Unexpected request size (expected 6, received {} bytes)", request.data.size()));
		break;
	case WRITE_MEMORY:
	{
		RequestTypes::MemoryWriteRequest memWriteReq{};
		response.resize(5);
		memWriteReq.address = 0;
		for (int i = 0; i < 4; ++i)
			memWriteReq.address |= (static_cast<unsigned char>(request.data[i]) << (i * 8));
		memWriteReq.bytes.insert(memWriteReq.bytes.begin(), request.data.begin() + 4, request.data.end());
		logger.logInfo(std::format("Received request for writing {} bytes of memory starting at address {:#x}", memWriteReq.bytes.size(), memWriteReq.address));

		// Submit request and wait for it to be fulfilled
		memWriteReq.fulfilled = false;
		if (requestQueue != nullptr)
		{
			RequestQueue::Request req;
			req.type = RequestQueue::Request::RequestType::MEMWRITE;
			req.data = &memWriteReq;
			requestQueue->push(std::move(req));

			using namespace std::chrono_literals;
			std::unique_lock<std::mutex> lock{memWriteReq.mutex};
			if (memWriteReq.cv.wait_for(lock, 1s, [&memWriteReq]() -> bool { return memWriteReq.fulfilled; }))
			{
				if (memWriteReq.errorCode == 0)
					response[0] = WRITE_MEMORY;
				else
					makeError(std::format("[5] WriteProcessMemory failed: error code {}", memWriteReq.errorCode));
			}
			else
				makeError("[5] Request timed out");
		}
		else
			makeError("[5] Request queue not initialized");
		break;
	}
	case QUERY_GAME_STATE:
	{
		response.push_back(QUERY_GAME_STATE);
		int type = request.data.empty() ? 0 : static_cast<int>(request.data.front());
		switch (type)
		{
		case 0: // Get current execution mode
			response.push_back(1);
			response.resize(5);
			response.push_back(static_cast<char>(currentGameMode.load()));
			break;
		case 1: // Get current map (internal) name
		{
			// Unsynchronized memory access...let's hope it doesn't cause any problems
			std::string_view mapName{csvanilla::gTMT[csvanilla::gStageNo].map};
			for (int i = 0; i < 4; ++i)
				response.push_back(static_cast<char>((mapName.size() >> (i * 8)) & 0xFF));
			response.insert(response.end(), mapName.begin(), mapName.end());
			break;
		}
		default:
			makeError(std::format("[6] Unknown request type {}", type));
		}

		break;
	}
	case DISCONNECT:
		logger.logInfo("Received disconnect signal; ending connection");
		connectionManager.stop(shared_from_this());
		break;
	default: // Unknown request! Uh-oh
		makeError(std::format("[{}] Unknown request type", static_cast<int>(request.header[0])));

		{
			std::ostringstream oss;
			oss << "Request data: " << std::hex;
			for (auto byte : request.data)
				oss << static_cast<int>(byte) << ' ';
			logger.logWarning(oss.str());
		}
	}
}
