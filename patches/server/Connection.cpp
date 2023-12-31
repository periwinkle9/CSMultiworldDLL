#include "Connection.h"
#include "ConnectionManager.h"
#include <utility>
#include <stdexcept>
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
		logger().logTrace("Socket is closed");
	}
	catch (std::exception& e)
	{
		logger().logDebug(std::format("Stopping connection due to exception: {}", e.what()));
		connectionManager.stop(shared_from_this());
	}
}

namespace
{
std::vector<std::int32_t> parseNumberList(const std::vector<char>& data)
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

std::string getServerInfoString()
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

enum class RequestFulfillStatus { Success, Canceled, TimedOut };
template <typename ReqType>
RequestFulfillStatus submitAndWaitForRequest(std::shared_ptr<ReqType>& requestData)
{
	Request request{requestData};
	requestQueue().push(std::move(request));

	using namespace std::chrono_literals;
	std::unique_lock<std::mutex> lock{requestData->mutex};
	if (requestData->cv.wait_for(lock, 1s, [&requestData]() -> bool { return requestData->fulfilled; }))
		return requestData->canceled ? RequestFulfillStatus::Canceled : RequestFulfillStatus::Success;
	else
		return RequestFulfillStatus::TimedOut;
}
} // end anonymous namespace

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
	{
		std::string script{request.data.begin(), request.data.end()};
		logger().logInfo("Queueing execution of script: " + script);
		eventQueue().push(std::move(script));

		response.push_back(EXEC_SCRIPT);
		response.resize(5);
		break;
	}
	case GET_FLAGS:
	{
		std::shared_ptr<RequestTypes::FlagReadRequest> flagRequest{new RequestTypes::FlagReadRequest};
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

		switch (submitAndWaitForRequest(flagRequest))
		{
		case RequestFulfillStatus::Success:
			response.insert(response.end(), flagRequest->result.begin(), flagRequest->result.end());
			break;
		case RequestFulfillStatus::Canceled:
			makeError("[2] Request canceled by server", Logger::LogLevel::Info);
			break;
		case RequestFulfillStatus::TimedOut:
			makeError("[2] Request timed out", Logger::LogLevel::Warning);
			break;
		}
		break;
	}
	case QUEUE_EVENTS:
	{
		std::vector<std::int32_t> eventList = parseNumberList(request.data);
		{
			std::ostringstream oss;
			oss << "Queueing execution of events: ";
			for (std::int32_t event : eventList)
				oss << event << ' ';
			logger().logInfo(oss.str());
		}
		eventQueue().push(eventList);

		response.push_back(QUEUE_EVENTS);
		response.resize(5);

		break;
	}
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

			switch (submitAndWaitForRequest(memReadReq))
			{
			case RequestFulfillStatus::Success:
				if (memReadReq->errorCode == 0)
					response.insert(response.end(), memReadReq->result.begin(), memReadReq->result.end());
				else
					makeError(std::format("[4] ReadProcessMemory (addr = {:#x}, size = {}) failed: error code {}",
						memReadReq->address, memReadReq->numBytes, memReadReq->errorCode), Logger::LogLevel::Warning);
				break;
			case RequestFulfillStatus::Canceled:
				makeError("[4] Request canceled by server", Logger::LogLevel::Info);
				break;
			case RequestFulfillStatus::TimedOut:
				makeError("[4] Request timed out", Logger::LogLevel::Warning);
				break;
			}
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

			switch (submitAndWaitForRequest(memWriteReq))
			{
			case RequestFulfillStatus::Success:
				if (memWriteReq->errorCode == 0)
					response[0] = WRITE_MEMORY;
				else
					makeError(std::format("[5] WriteProcessMemory (addr = {:#x}, size = {}) failed: error code {}",
						memWriteReq->address, memWriteReq->bytes.size(), memWriteReq->errorCode), Logger::LogLevel::Warning);
				break;
			case RequestFulfillStatus::Canceled:
				makeError("[5] Request canceled by server", Logger::LogLevel::Info);
				break;
			case RequestFulfillStatus::TimedOut:
				makeError("[5] Request timed out", Logger::LogLevel::Warning);
				break;
			}
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
			std::shared_ptr<RequestTypes::GetCurrentMapRequest> mapReq{new RequestTypes::GetCurrentMapRequest};
			switch (submitAndWaitForRequest(mapReq))
			{
			case RequestFulfillStatus::Success:
				writeInteger<std::uint32_t>(response, mapReq->mapName.size());
				response.insert(response.end(), mapReq->mapName.begin(), mapReq->mapName.end());
				break;
			case RequestFulfillStatus::Canceled:
				makeError("[6] Request canceled by server", Logger::LogLevel::Info);
				break;
			case RequestFulfillStatus::TimedOut:
				makeError("[6] Request timed out", Logger::LogLevel::Warning);
				break;
			}
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
		return;
	default: // Unknown request! Uh-oh
		makeError(std::format("[{}] Unknown request type", static_cast<int>(request.header[0])), Logger::LogLevel::Warning);

		{
			std::ostringstream oss;
			oss << "Request data: " << std::hex;
			for (unsigned char byte : request.data)
				oss << static_cast<unsigned>(byte) << ' ';
			logger().logInfo(oss.str());
		}
	}

	std::ostringstream oss;
	oss << std::hex;
	if (response.size() < 5)
	{
		logger().logError("Malformed server response, terminating connection");
		oss << "Response data: ";
		for (unsigned char byte : response)
			oss << static_cast<unsigned>(byte) << ' ';
		logger().logDebug(oss.str());
		throw std::logic_error("Malformed response");
	}
	else
	{
		oss << std::format("Sending response: type = {:d}, size = {}, data = ", response[0], readInteger<std::uint32_t>(response.cbegin() + 1));
		for (auto it = response.cbegin() + 5; it != response.cend(); ++it)
			oss << static_cast<unsigned>(static_cast<unsigned char>(*it)) << ' ';
		logger().logDebug(oss.str());
	}
}
} // end namespace csmulti
