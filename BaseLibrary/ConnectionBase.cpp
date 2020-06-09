#include "ConnectionBase.h"
#include <stdexcept>
#include "Ping.h"
#include "PingService.h"

int ConnectionBase::LastId = 0;

void ConnectionBase::subscribe(ConnectionObserver* observer) {
	if (observer)
		_observers.push_back(observer);
}

void ConnectionBase::unsubscribe(ConnectionObserver* observer) {
	_observers.remove(observer);
}

void ConnectionBase::onConnected() {
	writeSync(Ping());
	for (const auto& obs : _observers)
		if (isAlive())
			obs->onConnected(this);
}

void ConnectionBase::onDisconnected(const std::exception& exception) {
	cleanup();
	for (auto* obs : _observers)
		obs->onDisconnected(this, exception);
}

void ConnectionBase::onPayloadSent(const Serializable& payload, size_t size) {
	for (auto* obs : _observers)
		obs->onPayloadSent(this, payload, size);
}

void ConnectionBase::onPayloadReceived(const Serializable& payload, size_t size) {
	for (auto* obs : _observers)
		obs->onPayloadReceived(this, payload, size);
}

ConnectionBase::ConnectionBase(bool subscribeToPing) {
	_id = -1;
	_lastRequestId = 0;
	_socket = nullptr;
	if (subscribeToPing)
		PingService::getInstance().subscribe(this);
}

ConnectionBase::~ConnectionBase() {
	PingService::getInstance().unsubscribe(this);
	ConnectionBase::cleanup();
	_data.clear();
	_observers.clear();
}

void ConnectionBase::cleanup() {
	if (_socket) {
		p_socket_shutdown(_socket, true, true, nullptr);
	}
}

void ConnectionBase::close() {
	closeError(std::runtime_error("Connection closed"));
}

void ConnectionBase::assertConnected() const {
	if (!isAlive())
		throw std::runtime_error("Connection is closed");
}

ConnectionObserver::~ConnectionObserver() {
	_handlers.clear();
}

int ConnectionBase::newRequestId() {
	int id = ++_lastRequestId;
	// don't allow 0; wrap around
	return id ? id : ++_lastRequestId;
}


int ConnectionBase::writeRequestSync(TrackablePacket& payload) {
	int id = newRequestId();
	payload.setRequestId(id);
	writeSync(payload);
	return id;
}


void ConnectionBase::writeReplySync(TrackablePacket& payload, int requestId) {
	if (requestId)
		payload.setRequestId(requestId);
	writeSync(payload);
}

void ConnectionBase::writeReplySync(TrackablePacket& payload, const TrackablePacket& request) {
	writeReplySync(payload, request.getRequestId());
}
