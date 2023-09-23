#pragma once

#include <vector>
#include <array>
#include <memory>
#include <asio/awaitable.hpp>
#include <asio/ip/tcp.hpp>

class ConnectionManager;
class Connection: public std::enable_shared_from_this<Connection>
{
	asio::ip::tcp::socket socket;
	ConnectionManager& connectionManager;
	struct Request
	{
		std::array<unsigned char, 5> header;
		std::vector<char> data;
	} request;
	std::vector<char> response;

	asio::awaitable<void> handleRequest();
	void prepareResponse();
public:
	Connection(asio::ip::tcp::socket sock, ConnectionManager& manager);
	void start();
	void stop();
};
