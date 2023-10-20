#pragma once

#include <memory>
#include <set>
#include "Connection.h"

namespace csmulti
{
class ConnectionManager
{
	using ConnectionPtr = std::shared_ptr<Connection>;
	std::set<ConnectionPtr> connectionList;
public:
	ConnectionManager() = default;
	ConnectionManager(const ConnectionManager&) = delete;
	ConnectionManager& operator=(const ConnectionManager&) = delete;

	void start(ConnectionPtr newConnection)
	{
		connectionList.insert(newConnection);
		newConnection->start();
	}
	void stop(ConnectionPtr connection)
	{
		connectionList.erase(connection);
		connection->stop();
	}
	void stopAll()
	{
		for (auto& connection : connectionList)
			connection->stop();
		connectionList.clear();
	}
};
} // end namespace csmulti
