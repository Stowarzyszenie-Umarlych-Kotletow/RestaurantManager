#pragma once
#include "Connection.h"
#include <future>
#include <set>
#include <atomic>

class Server;
/**
 * Observer that allows listening to events of a Server object
 * 
 */
class ServerObserver : public ConnectionObserver {
protected:
	Server* _server;
public:
	ServerObserver() {
		_server = nullptr;
	}
	/**
	 * Gets the bound Server object
	 * 
	 * @return Server* 
	 */
	Server* getServer() const {
		return _server;
	}
	/**
	 * Sets the bound Server object
	 * 
	 * @param server 
	 */
	void setServer(Server* server) {
		_server = server;
	}
};

/**
 * Provides a TCP server with event-based interface
 */
class Server : protected ConnectionObserver {
protected:

	std::set<ServerObserver*> _observers;
	std::map<int, ConnectionBase*> _clients;
	std::set<ConnectionBase*> _graveyard;
	std::atomic<bool> _running;
	std::thread _listenThread;
	PSocket* _listenSocket;
	/**
	 * Predicate that should decide whether to accept the socket connection
	 * 
	 * @param socket accepted socket
	 * @return whether to accept the socket
	 */
	virtual bool shouldAcceptSocket(PSocket* socket);
	/**
	 * Internal function to setup the Connection object for the accepted socket
	 * 
	 * @param socket 
	 */
	virtual void prepareClient(PSocket* socket);
	/**
	 * Background task to accept incoming connections
	 * 
	 */
	virtual void listenWorker();

	/**
	 * Schedules a connection object to be deleted
	 * 
	 * @param connection object to be deleted
	 */
	virtual void deleteConnectionAsync(ConnectionBase* connection);

	/* Override ConnectionObserver interface */
	void onConnected(ConnectionBase* connection) override;
	void onDisconnected(ConnectionBase* connection, std::exception exception) override;
	void onPayloadSent(ConnectionBase* connection, const Serializable& payload, size_t size) override;
	void onPayloadReceived(ConnectionBase* connection, const Serializable& payload, size_t size) override;

public:
	/**
	 * Gets all connected clients
	 * @return std::map<int, ConnectionBase*> set of all clients
	 */
	virtual std::map<int, ConnectionBase*> clients() const {
		return _clients;
	}
	/**
	 * 
	 * @return true Whether the server is currently accepting incoming connections
	 */
	virtual bool isRunning() const {
		return _running;
	};

	Server();
	
	virtual ~Server();
	/**
	 * Starts listening for incoming connections on the specified endpoint
	 * 
	 * @param host IP address
	 * @param port port number
	 */
	virtual void start(const std::string& host, int port);
	/**
	 * Stops listening for incoming connections and drops all clients
	 * 
	 */
	virtual void stop();

	/**
	 * Sends a packet to all connected clients
	 * 
	 * @param msg payload
	 */
	virtual void writeToAll(const Serializable& msg);
	/**
	 * Joins the listener thread
	 * 
	 */
	virtual void join();
	/**
	 * Subscribes to the events of this server object 
	 * 
	 * @param observer 
	 */
	virtual void subscribe(ServerObserver* observer);
	/**
	 * Unsubscribes from the events of this server object
	 * 
	 * @param observer 
	 */
	virtual void unsubscribe(ServerObserver* observer);

};
