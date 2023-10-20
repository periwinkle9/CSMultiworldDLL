#include "server.h"
#include "Connection.h"
#include "ConnectionManager.h"
#include <functional>
#include <set>
#include <memory>
#include <format>
#include <stdexcept>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/post.hpp>
#include <asio/use_awaitable.hpp>
#include "../Multiworld.h"
#include "doukutsu/window.h"

using asio::awaitable;
using asio::ip::tcp;

namespace
{

// All of the ASIO examples that I've seen have this listener function as a free-standing function
// as opposed to being in a class. I guess I'll follow their example and do the same...
awaitable<void> listener(tcp::acceptor& acceptor, csmulti::ConnectionManager& connectionManager)
{
	while (acceptor.is_open())
	{
		csmulti::logger().logDebug("Awaiting connection");
		connectionManager.start(std::make_shared<csmulti::Connection>(co_await acceptor.async_accept(asio::use_awaitable), connectionManager));
	}
}

} // end anonymous namespace

namespace csmulti
{
Server::Server() : serverThread{}, io_context{}, acceptor{io_context}, activeConnections{}, isRunning{false}
{}
Server::Server(unsigned short port) : Server{}
{
	start(port);
}

void Server::start(unsigned short port)
{
	serverThread = std::thread([this, port]() {
		try
		{
			tcp::endpoint endpoint{tcp::v4(), port};
			acceptor.open(endpoint.protocol());
			acceptor.set_option(tcp::acceptor::reuse_address(true));
			acceptor.bind(endpoint);
			acceptor.listen();
			asio::co_spawn(io_context, listener(acceptor, activeConnections), asio::detached);
			logger().logInfo(std::format("Starting server on port {}", port));
			isRunning = true;
			io_context.run();
		}
		catch (const std::exception& e)
		{
			isRunning = false;
			logger().logError(std::format("Server terminated due to exception: {}", e.what()));
		}
	});
}

void Server::stop()
{
	if (isRunning)
	{
		logger().logInfo("Server stopping");
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
		logger().logWarning("Force-stopping server");
		io_context.stop(); // Kill the event loop; io_context.run() should return soon after this
		serverThread.join();
		isRunning = false;
	}
}
} // end namespace csmulti
