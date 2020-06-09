#include "Connection.h"
#include "net_constants.h"
#include "serialization.h"
#include "binary.h"
#include <future>
using namespace Binary;

int Connection::connect(const std::string& host, int port) {
	PSocketAddress* addr;
	PSocket* socket;

	if ((addr = p_socket_address_new(host.c_str(), port)) == nullptr) {
		throw std::runtime_error("Invalid hostname/port");
	}
	if ((socket = p_socket_new(P_SOCKET_FAMILY_INET, P_SOCKET_TYPE_STREAM, P_SOCKET_PROTOCOL_TCP, nullptr)) == nullptr
	) {
		p_socket_address_free(addr);
		throw std::runtime_error("Could not create a TCP socket");
	}
	PError* error;

	if (!p_socket_connect(socket, addr, &error)) {
		p_socket_address_free(addr);
		p_socket_free(socket);
		throw std::runtime_error("Could not establish connection");
	}

	p_socket_address_free(addr);
	return connect(socket);

}

int Connection::connect(PSocket* socket) {
	if (socket != _socket && isAlive())
		ConnectionBase::close();
	_socket = socket;
	if (isAlive()) {
		p_socket_set_keepalive(_socket, true);
		p_socket_set_timeout(_socket, SOCKET_TIMEOUT);
		_id = LastId++;
		onConnected();
	}
	return _id;
}

void Connection::closeError(const std::exception& exception) {
	if (_socket) {
		onDisconnected(exception);
	}
}

bool Connection::receiveFromSocket(size_t length) {
	auto& buf = _receiveStream.buffer();
	buf.setLength(length, true);
	buf.pubseekpos(0);
	auto* data = (pchar*)buf.data();

	pssize lastRead = 0;
	while (length > 0) {
		PError* error = nullptr;
		lastRead = p_socket_receive(_socket, data, length, &error);
		if (lastRead <= 0) {
			const char* msg = p_error_get_message(error);
			if (!msg)
				msg = "Connection broken (receive failed)";
			auto exception = std::runtime_error(msg);
			closeError(exception);
			throw exception;
		}
		data += lastRead;
		length -= lastRead;
	}

	return true;
}

bool Connection::sendBuffer() {
	auto& buf = _sendStream.buffer();
	auto* data = reinterpret_cast<pchar*>(buf.data() + static_cast<size_t>(buf.getInPosition()));
	size_t length = buf.getLength();

	pssize lastSent = 0;
	while (length > 0) {
		PError* error = nullptr;
		lastSent = p_socket_send(_socket, data, length, &error);
		if (lastSent <= 0) {
			const char* msg = p_error_get_message(error);
			if (!msg)
				msg = "Connection broken (send failed)";
			auto exception = std::runtime_error(msg);
			closeError(exception);
			throw exception;
		}
		data += lastSent;
		length -= lastSent;
	}

	return true;
}


void Connection::writeSync(const Serializable& payload) {

	assertConnected();
	auto& buf = _sendStream.buffer();
	buf.setLength(MAX_PACKET_SIZE + sizeof(content_len_t), true);
	buf.pubseekpos(sizeof(content_len_t));

	payload.serialize(_sendStream);

	const auto toWrite = static_cast<size_t>(buf.getOutPosition());
	const auto contentLength = static_cast<content_len_t>(toWrite) - sizeof(content_len_t);
	if (contentLength > MAX_PACKET_SIZE) {
		throw std::runtime_error("Exceeded max packet size");
	}
	buf.pubseekpos(0, std::ios_base::in | std::ios_base::out);
	write_primitive(_sendStream, contentLength);
	buf.setLength(toWrite, false);

	if (sendBuffer())
		onPayloadSent(payload, contentLength);


}

std::shared_ptr<Serializable> Connection::readSync() {
	if (_readingAsync)
		throw std::logic_error("This connection uses asynchronous reading");
	std::lock_guard<std::recursive_mutex> lock(_readLock);
	return readSyncInternal();
}

std::shared_ptr<Serializable> Connection::readSyncInternal() {
	assertConnected();
	if (receiveFromSocket(sizeof(content_len_t))) {
		content_len_t length;
		read_primitive(_receiveStream, &length);
		if (length > MAX_PACKET_SIZE) {
			// TODO: handle this by skipping bytes
			const auto exception = std::runtime_error("Exceeded max packet size");
			closeError(exception);
			throw exception;
		}
		// discard 'big' buffer if the required length fits in standard capacity
		auto& buf = _receiveStream.buffer();
		if (buf.getCapacity() > MAX_BUFFER_SIZE && length < MAX_BUFFER_SIZE)
			buf.dispose();

		if (receiveFromSocket(length)) {
			try {
				auto obj = new_instance(_receiveStream);
				obj->deserialize(_receiveStream);
				onPayloadReceived(*obj, length);
				return obj;
			}
			catch (std::runtime_error& ex) {
				std::cerr << ex.what() << std::endl;
				// invalid object found.. ignore it
			}
		}
	}
	return std::shared_ptr<Serializable>(nullptr);
}

bool Connection::readAsync() {
	std::lock_guard<std::recursive_mutex> lock(_readLock);

	_readingAsync = true;
	try {
		while (isAlive() && _readingAsync)
			auto _ = readSyncInternal();
	}
	catch (...) {
		// C++ has no real 'finally' support
	}
	_readingAsync = false;
	return true;
}

bool Connection::setReadingAsync(bool readAsync) {
	if (!_readingAsync && readAsync) {
		_readThread = std::thread([this] {
			this->readAsync();
		});
	}
	else if (_readingAsync && !readAsync) {
		std::lock_guard<std::recursive_mutex> lock(_readLock);
		if (_readThread.get_id() != std::this_thread::get_id() && _readThread.joinable())
			_readThread.join();
	}
	return readAsync;
}

void Connection::cleanup() {
	ConnectionBase::cleanup();
	_sendStream.dispose();
	_receiveStream.dispose();
	if (!p_socket_is_closed(_socket))
		p_socket_close(_socket, nullptr);
	p_socket_free(_socket);
	_socket = nullptr;
	setReadingAsync(false);
	if (_readThread.get_id() != std::this_thread::get_id() && _readThread.joinable()) {
		_readThread.join();
	} else {
		try {
			if(_readThread.get_id() != std::thread::id())
				_readThread.detach();
		}catch(...) {}
	}

}

Connection::~Connection() {
	Connection::cleanup();
}
