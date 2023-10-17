#include "server.h"
#include "Connection.h"
#include "ConnectionManager.h"
#include <functional>
#include <set>
#include <memory>
#include <format>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/post.hpp>
#include <asio/use_awaitable.hpp>
#include "../Logger.h"
#include "../Config.h"
#include "doukutsu/window.h"

using asio::awaitable;
using asio::ip::tcp;

Server* tcpServer;

namespace
{

// All of the ASIO examples that I've seen have this listener function as a free-standing function
// as opposed to being in a class. I guess I'll follow their example and do the same...
awaitable<void> listener(tcp::acceptor& acceptor, ConnectionManager& connectionManager)
{
	while (acceptor.is_open())
	{
		logger.logDebug("Awaiting connection");
		connectionManager.start(std::make_shared<Connection>(co_await acceptor.async_accept(asio::use_awaitable), connectionManager));
	}
}

} // end anonymous namespace

// Delay initialization to avoid calling the constructor in DllMain()
void initServer()
{
	tcpServer = new Server(config.serverPort());
}
void endServer()
{
	tcpServer->stop();
	delete tcpServer;
	tcpServer = nullptr;
}

Server::Server() : serverThread{}, io_context{}, acceptor{io_context}, activeConnections{}, isRunning{false}
{}
Server::Server(unsigned short port) : Server{}
{
	start(port);
}

void Server::start(unsigned short port)
{
	serverThread = std::thread([this, port]() {
		tcp::endpoint endpoint{tcp::v4(), port};
		acceptor.open(endpoint.protocol());
		acceptor.set_option(tcp::acceptor::reuse_address(config.allowReuseAddress()));
		acceptor.bind(endpoint);
		acceptor.listen();
		asio::co_spawn(io_context, listener(acceptor, activeConnections), asio::detached);
		logger.logInfo(std::format("Starting server on port {}", port));
		io_context.run();
	});
	isRunning = true;
}

void Server::stop()
{
	if (isRunning)
	{
		logger.logInfo("Server stopping");
		// Post connection stop request
		asio::post(io_context, [this]() {
			acceptor.close();
			activeConnections.stopAll();
			});
		// Wait until everything shuts down and io_context.run() returns
		serverThread.join();
		isRunning = false;
	}
}

void Server::forceStop()
{
	if (isRunning)
	{
		logger.logWarning("Force-stopping server");
		io_context.stop(); // Kill the event loop; io_context.run() should return soon after this
		serverThread.join();
		isRunning = false;
	}
}
