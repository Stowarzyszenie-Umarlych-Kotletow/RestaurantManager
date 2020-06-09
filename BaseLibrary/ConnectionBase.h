#include "DataBag.h"
#include "Serializable.h"
#include "plibsys.h"
#include <functional>
#include <set>
#include <atomic>
#include <list>
#include <mutex>
#include "TrackablePacket.h"
using namespace std::placeholders;
#pragma once

class ConnectionBase;

/**
 * Observer that allows listening to events of a Connection object
 * 
 */
class ConnectionObserver {
protected:
	std::map<Type, std::list<std::function<void(ConnectionBase*, const Serializable&, size_t)>>> _handlers;
public:
	virtual ~ConnectionObserver();
	/**
	 * Registers a payload handler
	 * 
	 * @tparam T type of payload 
	 * @param callback handler
	 */
	template <typename T>
	void addHandler(std::function<void(ConnectionBase*, const T&, size_t)> callback) {
		_handlers[get_type<T>()].push_back([callback](ConnectionBase* con, const Serializable& data, size_t size) {
			callback(con, dynamic_cast<const T&>(data), size);
		});
	}
	/**
	 * Registers a payload handler (wrapper for inherited member methods)
	 * 
	 * @tparam TBase type containing the method
	 * @tparam T type of payload
	 * @param callback handler (member method)
	 */
	template <typename TBase, typename T>
	void addHandler(void (TBase::* callback)(ConnectionBase*, const T&, size_t)) {
		addHandler<T>(std::bind(callback, static_cast<TBase*>(this), _1, _2, _3));
	}

	/**
	 * Clears handlers for the specified payload type
	 * 
	 * @tparam T type of payload
	 */
	template <typename T>
	void removeHandlers() {
		_handlers[get_type<T>()].clear();
	}
	/**
	 * Event handler for connection established
	 * 
	 * @param connection event source
	 */
	virtual void onConnected(ConnectionBase* connection) {}
	/**
	 * Event handler for connection closed
	 * 
	 * @param connection event source
	 * @param exception connection exception
	 */
	virtual void onDisconnected(ConnectionBase* connection, std::exception exception) {}
	/**
	 * Event handler for payload sent
	 * 
	 * @param connection event source
	 * @param payload sent payload
	 * @param size size (in bytes) of sent payload
	 */
	virtual void onPayloadSent(ConnectionBase* connection, const Serializable& payload, size_t size) {}
	/**
	 * Event handler for payload received
	 * 
	 * @param connection event source
	 * @param payload received payload
	 * @param size size (in bytes) of received payload
	 */
	virtual void onPayloadReceived(ConnectionBase* connection, const Serializable& payload, size_t size) {
		auto it = _handlers.find(payload.getType());
		if (it != _handlers.end()) {
			for (const auto& callback : it->second) {
				callback(connection, payload, size);
			}
		}
	}
};

/**
 * Abstract class implementing basic functionality for wrapping a PSocket
 * into a managed TCP Client
 */
class ConnectionBase {
protected:
	static int LastId;

	PSocket* _socket;
	DataBag _data;
	std::list<ConnectionObserver*> _observers;
	std::atomic<bool> _readingAsync;
	int _id;
	int _lastRequestId;
	/**
	 * Generates a new request id
	 * 
	 * @return int request id
	 */
	virtual int newRequestId();
	/**
	 * Event handler for connection established
	 * 
	 */
	virtual void onConnected();
	/**
	 * Event handler for connection closed
	 * 
	 * @param exception connection exception
	 */
	virtual void onDisconnected(const std::exception& exception);
	/**
	 * Event handler for payload sent
	 * 
	 * @param payload sent payload
	 * @param size size (in bytes) of sent payload
	 */
	virtual void onPayloadSent(const Serializable& payload, size_t size);
	/**
	 * Event handler for payload received
	 * 
	 * @param payload received payload
	 * @param size size (in bytes) of received payload
	 */
	virtual void onPayloadReceived(const Serializable& payload, size_t size);
	/**
	 * Clean-up method called after the client is disconnected
	 * 
	 */
	virtual void cleanup();
	/**
	 * Closes the connection as a result of the given exception
	 * 
	 * @param exception caught exception
	 */
	virtual void closeError(const std::exception& exception) = 0;
	/**
	 * Background task to receive payload and handle all connection events
	 * 
	 * @return true exited properly
	 * @return false exited with an exception
	 */
	virtual bool readAsync() = 0;
	/**
	 * Throws an error if the connection is not alive
	 * 
	 */
	virtual void assertConnected() const;

public:	
	/**
	 * @return Whether the connection is alive and connected
	 */
	virtual bool isAlive() const = 0;
	/**
	 * Gets the DataBag associated with this connection by reference
	 * 
	 * @return DataBag& 
	 */
	virtual DataBag& getData() {
		return _data;
	}
	/**
	 * Gets the DataBag associated with this connection by const reference
	 * 
	 * @return const DataBag& 
	 */
	virtual const DataBag& getData() const {
		return _data;
	}
	/**
	 * Gets the unique id of this connection
	 * 
	 * @return int connection id
	 */
	virtual int getId() const {
		return _id;
	}
	/**
	 * 
	 * @return Whether all input is being processed in a background thread
	 */
	virtual bool isReadingAsync() const {
		return _readingAsync;
	}
	/**
	 * Controls whether all input should be processed in a background thread
	 * 
	 * @param readAsync 
	 * @return whether the background receive task is now running
	 */
	virtual bool setReadingAsync(bool readAsync) = 0;
	/**
	 * Construct a new ConnectionBase object
	 * 
	 * @param subscribeToPing whether to subscribe to the PingService
	 */
	ConnectionBase(bool subscribeToPing);
	virtual ~ConnectionBase();
	/**
	 * Connects to the target endpoint
	 * 
	 * @param host hostname, DNS is not supported
	 * @param port port number
	 * @return int connection id
	 */
	virtual int connect(const std::string& host, int port) = 0;
	/**
	 * Wraps the existing socket object
	 * 
	 * @param socket client socket
	 * @return int connection id
	 */
	virtual int connect(PSocket* socket) = 0;
	/**
	 * Closes this connection and releases all resources
	 * 
	 */
	virtual void close();
	/**
	 * Subscribes an event observer to this connection
	 * 
	 * @param observer event observer
	 */
	virtual void subscribe(ConnectionObserver* observer);
	/**
	 * Unsubscribes an event observer to this connection
	 * 
	 * @param observer event observer
	 */
	virtual void unsubscribe(ConnectionObserver* observer);
	/**
	 * Synchronously writes the given payload into the network stream
	 * 
	 * @param payload payload object
	 */
	virtual void writeSync(const Serializable& payload) = 0;
	/**
	 * Synchronously writes the given request into the network stream
	 * 
	 * @param payload request packet
	 * @return int unique id of the sent request
	 */
	virtual int writeRequestSync(TrackablePacket& payload);
	/**
	 * Synchronously writes the given reply into the network stream
	 * 
	 * @param payload response packet
	 * @param requestId id of the request
	 */
	virtual void writeReplySync(TrackablePacket& payload, int requestId = 0);
	/**
	 * Synchronously writes the given reply into the network stream
	 * 
	 * @param payload response packet
	 * @param request request packet
	 */
	virtual void writeReplySync(TrackablePacket& payload, const TrackablePacket& request);
	/**
	 * Synchronously reads a single packet from the network stream
	 * 
	 * @return std::shared_ptr<Serializable> payload received
	 */
	virtual std::shared_ptr<Serializable> readSync() = 0;

};
