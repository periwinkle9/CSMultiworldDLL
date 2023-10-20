#include "Connection.h"
#include "ConnectionManager.h"
#include <utility>
#include <exception>
#include <cstdint>
#include <sstream>
#include <format>
#include <string_view>
#include <chrono>
#include <memory>
#include <asio/buffer.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/read.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>
#include "../request/RequestQueue.h"
#include "../request/RequestTypes.h"
#include "../Multiworld.h"
#include "doukutsu/flags.h"
#include "doukutsu/inventory.h"
#include "doukutsu/map.h"
#include "doukutsu/player.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using asio::ip::tcp;

namespace csmulti
{
Connection::Connection(tcp::socket sock, ConnectionManager& manager) :
	socket(std::move(sock)), connectionManager(manager), request(), response()
{}

void Connection::start()
{
	logger().logDebug("Connection established, starting request handler");
	asio::co_spawn(socket.get_executor(), [self = shared_from_this()]{ return self->handleRequest(); }, asio::detached);
}

void Connection::stop()
{
	logger().logDebug("Shutting down connection");
	socket.shutdown(tcp::socket::shutdown_both);
	socket.close();
}

namespace
{
// Reads an integer in little-endian format from the bytes starting from the given position
template <typename IntType, typename Iter>
IntType readInteger(Iter pos)
{
	IntType result = 0;
	for (std::size_t i = 0; i < sizeof(IntType); ++i)
		result |= (static_cast<std::uint8_t>(*pos++) << (i * 8));
	return result;
}
// Appends the little-endian representation of the given value
template <typename IntType>
void writeInteger(std::vector<char>& data, IntType value)
{
	for (std::size_t i = 0; i < sizeof(IntType); ++i)
		data.push_back(static_cast<char>((value >> (i * 8)) & 0xFF));
}
} // end anonymous namespace

asio::awaitable<void> Connection::handleRequest()
{
	try
	{
		while (socket.is_open())
		{
			// Read header
			co_await asio::async_read(socket, asio::buffer(request.header), asio::use_awaitable);
			std::uint32_t dataSize = readInteger<std::uint32_t>(request.header.cbegin() + 1);
			request.data.clear();

			// Read data
			if (dataSize > 0)
			{
				// Warn on unusually large request (bad format?)
				if (dataSize > 1024 * 1024) // 1 MiB
				{
					logger().logWarning(std::format("Received unusually large request of type {:d}, size = {} (wrong format?)",
						request.header[0], dataSize));
					logger().logWarning("This operation will block forever if this is not the actual amount of data being sent");
				}
				request.data.resize(dataSize);
				co_await asio::async_read(socket, asio::buffer(request.data), asio::use_awaitable);
			}

			prepareResponse();

			// Send reply
			if (!response.empty())
				co_await asio::async_write(socket, asio::buffer(response), asio::use_awaitable);
		}
		logger().logDebug("Socket is closed");
	}
	catch (std::exception& e)
	{
		logger().logDebug(std::format("Stopping connection due to exception: {}", e.what()));
		connectionManager.stop(shared_from_this());
	}
}

static std::vector<std::int32_t> parseNumberList(const std::vector<char>& data)
{
	if (data.size() % 4 != 0)
		logger().logWarning(std::format("Received data not a multiple of 4 bytes (size = {}); ignoring the last {} bytes", data.size(), data.size() % 4));
	std::vector<std::int32_t> output;
	output.reserve(data.size() / 4);
	for (std::vector<char>::size_type i = 0; i + 3 < data.size(); i += 4)
	{
		std::int32_t value = readInteger<std::int32_t>(data.cbegin() + i);
		output.push_back(value);
	}
	return output;
}

static std::string getServerInfoString()
{
	constexpr int API_Version = 0;
	// Manually construct JSON string :')
	constexpr std::string_view outputFormat = R"!!({{
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
	return std::format(outputFormat, API_Version, "freeware", uuid().string(),
		reinterpret_cast<std::uint32_t>(&csvanilla::gFlagNPC),
		reinterpret_cast<std::uint32_t>(&csvanilla::gMapping),
		reinterpret_cast<std::uint32_t>(&csvanilla::gArmsData),
		reinterpret_cast<std::uint32_t>(&csvanilla::gMC.life),
		reinterpret_cast<std::uint32_t>(&csvanilla::gMC.max_life));
}

void Connection::prepareResponse()
{
	enum RequestType : unsigned char { HANDSHAKE, EXEC_SCRIPT, GET_FLAGS, QUEUE_EVENTS, READ_MEMORY, WRITE_MEMORY, QUERY_GAME_STATE, DISCONNECT = 255 };

	{
		logger().logInfo(std::format("Received request: type = {:d}, size = {}", request.header[0], request.data.size()));
		std::ostringstream oss;
		oss << "Raw request data: " << std::hex;
		for (auto byte : request.data)
			oss << static_cast<unsigned>(static_cast<unsigned char>(byte)) << ' ';
		logger().logDebug(oss.str());
	}

	auto makeError = [this](std::string_view message, Logger::LogLevel logLevel = Logger::LogLevel::Error) {
		response.clear();
		response.push_back(-1);
		writeInteger<std::uint32_t>(response, message.size());
		response.insert(response.end(), message.begin(), message.end());
		logger().log(logLevel, std::string{message});
	};
	response.clear();
	switch (request.header[0])
	{
	case HANDSHAKE:
	{
		std::string serverInfo = getServerInfoString();
		response.push_back(HANDSHAKE);
		writeInteger<std::uint32_t>(response, serverInfo.size());
		response.insert(response.end(), serverInfo.begin(), serverInfo.end());
		break;
	}
	case EXEC_SCRIPT:
		if (requestQueue() != nullptr)
		{
			RequestQueue::Request event;
			event.type = RequestQueue::Request::RequestType::SCRIPT;
			event.data = std::string(request.data.begin(), request.data.end());

			logger().logInfo("Queueing execution of script: " + std::any_cast<std::string>(event.data));
			requestQueue()->push(std::move(event));

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
		std::shared_ptr<RequestTypes::FlagRequest> flagRequest{new RequestTypes::FlagRequest};
		flagRequest->flags = parseNumberList(request.data);
		{
			std::ostringstream oss;
			oss << "Received request for flags: ";
			for (std::int32_t flag : flagRequest->flags)
				oss << flag << ' ';
			logger().logInfo(oss.str());
		}

		response.reserve(flagRequest->flags.size() + 5);
		response.push_back(GET_FLAGS);
		writeInteger<std::uint32_t>(response, flagRequest->flags.size());

		// Submit request and wait for it to be fulfilled
		flagRequest->fulfilled = false;
		if (requestQueue() != nullptr)
		{
			RequestQueue::Request req;
			req.type = RequestQueue::Request::RequestType::FLAGS;
			req.data = flagRequest;
			requestQueue()->push(std::move(req));

			using namespace std::chrono_literals;
			std::unique_lock<std::mutex> lock{flagRequest->mutex};
			if (flagRequest->cv.wait_for(lock, 1s, [&flagRequest]() -> bool { return flagRequest->fulfilled; }))
				response.insert(response.end(), flagRequest->result.begin(), flagRequest->result.end());
			else
				makeError("[2] Request timed out", Logger::LogLevel::Warning);
		}
		else
			makeError("[2] Request queue not initialized");

		break;
	}
	case QUEUE_EVENTS:
		if (requestQueue() != nullptr)
		{
			std::vector<std::int32_t> eventList = parseNumberList(request.data);
			{
				std::ostringstream oss;
				oss << "Queueing execution of events: ";
				for (std::int32_t event : eventList)
					oss << event << ' ';
				logger().logInfo(oss.str());
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
			requestQueue()->pushMultiple(eventRequests);

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
			std::shared_ptr<RequestTypes::MemoryReadRequest> memReadReq{new RequestTypes::MemoryReadRequest};
			memReadReq->address = readInteger<std::uint32_t>(request.data.cbegin());
			memReadReq->numBytes = readInteger<std::uint16_t>(request.data.cbegin() + 4);
			logger().logInfo(std::format("Received request for {} bytes of memory starting at address {:#x}", memReadReq->numBytes, memReadReq->address));
			response.push_back(READ_MEMORY);
			response.push_back(request.data[4]);
			response.push_back(request.data[5]);
			response.resize(5);

			// Submit request and wait for it to be fulfilled
			memReadReq->fulfilled = false;
			if (requestQueue() != nullptr)
			{
				RequestQueue::Request req;
				req.type = RequestQueue::Request::RequestType::MEMREAD;
				req.data = memReadReq;
				requestQueue()->push(std::move(req));

				using namespace std::chrono_literals;
				std::unique_lock<std::mutex> lock{memReadReq->mutex};
				if (memReadReq->cv.wait_for(lock, 1s, [&memReadReq]() -> bool { return memReadReq->fulfilled; }))
				{
					if (memReadReq->errorCode == 0)
						response.insert(response.end(), memReadReq->result.begin(), memReadReq->result.end());
					else
						makeError(std::format("[4] ReadProcessMemory (addr = {:#x}, size = {}) failed: error code {}",
							memReadReq->address, memReadReq->numBytes, memReadReq->errorCode), Logger::LogLevel::Warning);
				}
				else
					makeError("[4] Request timed out", Logger::LogLevel::Warning);
			}
			else
				makeError("[4] Request queue not initialized");
		}
		else
			makeError(std::format("[4] Unexpected request size (expected 6, received {} bytes)", request.data.size()), Logger::LogLevel::Warning);
		break;
	case WRITE_MEMORY:
		if (request.data.size() > 4)
		{
			std::shared_ptr<RequestTypes::MemoryWriteRequest> memWriteReq{new RequestTypes::MemoryWriteRequest};
			response.resize(5);
			memWriteReq->address = readInteger<std::uint32_t>(request.data.cbegin());
			memWriteReq->bytes.insert(memWriteReq->bytes.begin(), request.data.begin() + 4, request.data.end());
			logger().logInfo(std::format("Received request for writing {} bytes of memory starting at address {:#x}", memWriteReq->bytes.size(), memWriteReq->address));

			// Submit request and wait for it to be fulfilled
			memWriteReq->fulfilled = false;
			if (requestQueue() != nullptr)
			{
				RequestQueue::Request req;
				req.type = RequestQueue::Request::RequestType::MEMWRITE;
				req.data = memWriteReq;
				requestQueue()->push(std::move(req));

				using namespace std::chrono_literals;
				std::unique_lock<std::mutex> lock{memWriteReq->mutex};
				if (memWriteReq->cv.wait_for(lock, 1s, [&memWriteReq]() -> bool { return memWriteReq->fulfilled; }))
				{
					if (memWriteReq->errorCode == 0)
						response[0] = WRITE_MEMORY;
					else
						makeError(std::format("[5] WriteProcessMemory (addr = {:#x}, size = {}) failed: error code {}",
							memWriteReq->address, memWriteReq->bytes.size(), memWriteReq->errorCode), Logger::LogLevel::Warning);
				}
				else
					makeError("[5] Request timed out", Logger::LogLevel::Warning);
			}
			else
				makeError("[5] Request queue not initialized");
		}
		else if (request.data.size() == 4)
			makeError("[5] Requested to write 0 bytes of memory", Logger::LogLevel::Warning);
		else
			makeError(std::format("[5] Unexpected request size (expected > 4 bytes, received {} bytes", request.data.size()), Logger::LogLevel::Warning);
		break;
	case QUERY_GAME_STATE:
	{
		response.push_back(QUERY_GAME_STATE);
		int type = request.data.empty() ? 0 : static_cast<int>(request.data.front());
		switch (type)
		{
		case 0: // Get current execution mode
			response.push_back(1);
			response.resize(5);
			response.push_back(static_cast<char>(Multiworld::getInstance().currentGameMode()));
			break;
		case 1: // Get current map (internal) name
		{
			// Unsynchronized memory access...let's hope it doesn't cause any problems
			std::string_view mapName{csvanilla::gTMT[csvanilla::gStageNo].map};
			writeInteger<std::uint32_t>(response, mapName.size());
			response.insert(response.end(), mapName.begin(), mapName.end());
			break;
		}
		default:
			makeError(std::format("[6] Unknown request type {}", type), Logger::LogLevel::Warning);
		}

		break;
	}
	case DISCONNECT:
		logger().logInfo("Received disconnect signal; ending connection");
		connectionManager.stop(shared_from_this());
		break;
	default: // Unknown request! Uh-oh
		makeError(std::format("[{}] Unknown request type", static_cast<int>(request.header[0])), Logger::LogLevel::Warning);

		{
			std::ostringstream oss;
			oss << "Request data: " << std::hex;
			for (auto byte : request.data)
				oss << static_cast<int>(byte) << ' ';
			logger().logInfo(oss.str());
		}
	}
}
} // end namespace csmulti
