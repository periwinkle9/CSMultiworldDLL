#pragma once

#include <thread>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include "ConnectionManager.h"

namespace csmulti
{
class Server
{
	std::thread serverThread;
	asio::io_context io_context;
	asio::ip::tcp::acceptor acceptor;
	ConnectionManager activeConnections;
	bool isRunning;

public:
	Server(); // Construct server without starting it
	Server(unsigned short port); // Construct server and start it immediately on this port

	void start(unsigned short port);
	void stop();
	void forceStop(); // Just in case
};

extern Server* tcpServer;
}

void initServer();
void endServer();
