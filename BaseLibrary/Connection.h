#pragma once
#include "ConnectionBase.h"
#include "plibsys.h"
#include "types.h"
#include "buffers.h"
#include <thread>
#include <mutex>


/**
 * Main class that wraps a native socket into a managed TCP Client
 * 
 */
class Connection : public ConnectionBase {
protected:
	memory_stream _sendStream, _receiveStream;
	std::recursive_mutex _readLock;
	std::thread _readThread;

	void cleanup() override;
	void closeError(const std::exception& exception) override;
	/**
	 * Helper method to read length bytes from input into the buffer
	 * 
	 * @param length number of bytes to be read
	 * @return true success
	 * @return false failure
	 */
	bool receiveFromSocket(size_t length);
	/**
	 * Helper method to push bytes stored in the send buffer
	 * 
	 * @return true success
	 * @return false failure
	 */
	bool sendBuffer();
	bool readAsync() override;
	/**
	 * Internal function to read a single packet from the sream
	 * 
	 * @return std::shared_ptr<Serializable> payload received
	 */
	virtual std::shared_ptr<Serializable> readSyncInternal();

public:
	bool isAlive() const override {
		return _socket && p_socket_is_connected(_socket);
	}

	bool setReadingAsync(bool readAsync) override;
	/**
	 * Construct a new Connection object
	 * 
	 * @param subscribeToPing whether to subscribe to the PingService
	 */
	Connection(bool subscribeToPing = true) : ConnectionBase(subscribeToPing) {}
	virtual ~Connection();
	int connect(const std::string& host, int port) override;
	int connect(PSocket* socket) override;
	void writeSync(const Serializable& payload) override;
	std::shared_ptr<Serializable> readSync() override;

};
