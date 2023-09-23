#include "Connection.h"
#include "ConnectionManager.h"
#include <utility>
#include <exception>
#include <cstdint>
#include <iostream>
#include <asio/buffer.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/read.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>
#include "../RequestQueue.h"
#include "doukutsu/flags.h"

using asio::ip::tcp;

Connection::Connection(tcp::socket sock, ConnectionManager& manager) :
	socket(std::move(sock)), connectionManager(manager), request(), response()
{}

void Connection::start()
{
	std::cout << "Connection established, starting request handler" << std::endl;
	asio::co_spawn(socket.get_executor(), [self = shared_from_this()]{return self->handleRequest();}, asio::detached);
}

void Connection::stop()
{
	std::cout << "Shutting down connection" << std::endl;
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

void Connection::prepareResponse()
{
	enum RequestType: unsigned char {ECHO, EXEC_SCRIPT, GET_FLAGS, QUEUE_EVENTS, DISCONNECT = 255};

	std::cout << "Received request: type = " << static_cast<int>(request.header[0]) << ", size = " << request.data.size() << std::endl;
	/*
	std::cout << "Raw request data: " << std::hex;
	for (auto byte : request.data)
		std::cout << static_cast<unsigned>(static_cast<unsigned char>(byte)) << ' ';
	std::cout << std::dec << std::endl;
	*/

	response.clear();
	switch (request.header[0])
	{
	case ECHO:
		// For now, let's just send the exact same thing back
		std::cout << "Echo message: " << std::string(request.data.begin(), request.data.end()) << std::endl;
		response = std::move(request.data);
		response.insert(response.begin(), request.header.begin(), request.header.end());
		break;
	case EXEC_SCRIPT:
		if (requestQueue != nullptr)
		{
			RequestQueue::Request event;
			event.type = RequestQueue::Request::RequestType::SCRIPT;
			event.script = std::string(request.data.begin(), request.data.end());
			
			std::cout << "Queueing execution of script: " << event.script << std::endl;
			requestQueue->push(std::move(event));
			
			response.push_back(EXEC_SCRIPT);
			response.resize(5);
		}
		else
		{
			// Request queue not available? That's not good...
			std::cout << "Failed to queue script execution: event queue not initialized" << std::endl;
			response.push_back(-1); // Send back error status
			response.resize(5);
		}
		break;
	case GET_FLAGS:
	{
		std::vector<std::int32_t> flagList = parseNumberList(request.data);
		std::cout << "Received request for flags: ";
		for (std::int32_t flag : flagList)
			std::cout << flag << ' ';
		std::cout << std::endl;

		response.reserve(flagList.size() + 5);
		response.push_back(GET_FLAGS);
		// Write response length
		for (int i = 0; i < 4; ++i)
			response.push_back(static_cast<char>((flagList.size() >> (i * 8)) & 0xFF));

		// This gFlagNPC access is completely unsynchronized. Is that a problem?
		auto getFlag = [](std::int32_t flagNum) -> bool { return (csvanilla::gFlagNPC[flagNum / 8] & (1 << (flagNum % 8))) != 0; };
		for (auto flag : flagList)
			response.push_back(getFlag(flag));
		break;
	}
	case QUEUE_EVENTS:
		if (requestQueue != nullptr)
		{
			std::vector<std::int32_t> eventList = parseNumberList(request.data);
			std::cout << "Queueing execution of events: ";
			for (std::int32_t event : eventList)
				std::cout << event << ' ';
			std::cout << std::endl;

			std::vector<RequestQueue::Request> eventRequests;
			eventRequests.reserve(eventList.size());
			for (auto eventNum : eventList)
			{
				RequestQueue::Request req;
				req.type = RequestQueue::Request::RequestType::EVENTNUM;
				req.eventNum = eventNum;
				eventRequests.push_back(req);
			}
			requestQueue->pushMultiple(eventRequests);

			response.push_back(QUEUE_EVENTS);
			response.resize(5);
		}
		else
		{
			// Request queue not available? That's not good...
			std::cout << "Failed to queue script execution: event queue not initialized" << std::endl;
			response.push_back(-1); // Send back error status
			response.resize(5);
		}
		break;
	case DISCONNECT:
		std::cout << "Received disconnect signal; ending connection" << std::endl;
		connectionManager.stop(shared_from_this());
		break;
	default: // Unknown request! Uh-oh
		std::cout << "Received unknown request! Request data: " << std::hex;
		for (auto byte : request.data)
			std::cout << static_cast<int>(byte) << ' ';
		std::cout << std::dec << std::endl;

		response.push_back(-1); // Send back error status
		response.resize(5);
	}
}
