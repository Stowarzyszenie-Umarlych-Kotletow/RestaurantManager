#include "PingService.h"
#include "net_constants.h"

PingService PingService::_instance;

void PingService::worker() {
	auto delay = std::chrono::milliseconds(_timeout);
	while (_run) {
		{
			std::lock_guard<std::recursive_mutex> guard(_lock);
			for (auto it = _connections.begin(); it != _connections.end();) {
				try {
					auto* con = *it;
					if (con->isAlive())
						con->writeSync(Ping());
					++it;
				}
				catch (...) { }
			}
		}
		std::this_thread::sleep_for(delay);
	}
}

PingService::PingService(int timeout) {
	_timeout = timeout;
}

void PingService::start() {
	if (_run)
		throw std::logic_error("Already started");
	_run = true;
	
	auto thread = std::thread([this](int delay) {
		this->worker();
		}, _timeout);
	thread.detach();
}

void PingService::stop() {
	_run = false;
}

void PingService::subscribe(ConnectionBase* connection) {
	std::lock_guard<std::recursive_mutex> guard(_lock);
	_connections.insert(connection);
}

void PingService::unsubscribe(ConnectionBase* connection) {
	std::lock_guard<std::recursive_mutex> guard(_lock);
	_connections.erase(connection);
}
