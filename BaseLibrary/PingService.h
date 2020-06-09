#pragma once
#include "ConnectionBase.h"
#include "net_constants.h"
#include "Ping.h"
#include <set>
#include <chrono>
#include <stdexcept>
#include <mutex>
/**
 * Singleton class to broadcast Ping packets on all bound connections
 * 
 */
class PingService {
protected:
	std::atomic_bool _run;
	std::set<ConnectionBase*> _connections;
	std::recursive_mutex _lock;
	int _timeout;
	static PingService _instance;
	/**
	 * Background task that broadcasts the Ping packets
	 * 
	 */
	void worker();

public:
	/**
	 * Construct a new Ping Service object
	 * 
	 * @param timeout Millisecond interval to send the Ping packets
	 */
	PingService(int timeout = CLIENT_PING_INTERVAL);
	/**
	 * Starts the PingService
	 */
	void start();
	/**
	 * Stops the PingService
	 */
	void stop();
	
	/**
	 * Binds the connection to this service
	 * 
	 * @param connection 
	 */
	void subscribe(ConnectionBase* connection);
	/**
	 * Unbinds the connection from this service
	 * 
	 * @param connection 
	 */
	void unsubscribe(ConnectionBase* connection);
	/**
	 * Gets the instance of this object
	 * 
	 * @return PingService& 
	 */
	static PingService& getInstance() {
		return _instance;
	}

};
