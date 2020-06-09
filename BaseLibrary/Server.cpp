#include "Server.h"


bool Server::shouldAcceptSocket(PSocket* socket) {
	return true;
}


void Server::prepareClient(PSocket* socket) {
	auto client = static_cast<ConnectionBase*>(new Connection(false));
	client->subscribe(this);
	client->connect(socket);
	if (!client->isAlive()) {
		client->close();
		return;
	}
	_clients[client->getId()] = client;
	client->setReadingAsync(true);
}

void Server::listenWorker() {
	while (_listenSocket && _running && !p_socket_is_closed(_listenSocket)) {
		PSocket* client = p_socket_accept(_listenSocket, nullptr);

		if (!client)
			continue;

		if (shouldAcceptSocket(client)) {
			std::thread thr([this, client]() 
			{
				prepareClient(client);
			});
			thr.detach();
		}
	}
}

void Server::onConnected(ConnectionBase* connection) {
	for (auto obs : _observers) {
		obs->onConnected(connection);
	}
}

void Server::onDisconnected(ConnectionBase* connection, std::exception exception) {
	for (auto obs : _observers) {
		obs->onDisconnected(connection, exception);
	}
	int id = connection->getId();
	_clients.erase(id);
	this->deleteConnectionAsync(connection);
}

void Server::onPayloadSent(ConnectionBase* connection, const Serializable& payload, size_t size) {
	for (auto obs : _observers) {
		obs->onPayloadSent(connection, payload, size);
	}
}

void Server::onPayloadReceived(ConnectionBase* connection, const Serializable& payload, size_t size) {
	ConnectionObserver::onPayloadReceived(connection, payload, size);
	for (auto obs : _observers) {
		obs->onPayloadReceived(connection, payload, size);
	}
}

void Server::start(const std::string& host, int port) {
	if (_running)
		throw std::logic_error("Server is already running");
	PSocketAddress* addr;
	PSocket* sock;

	if ((addr = p_socket_address_new(host.c_str(), port)) == nullptr)
		throw std::runtime_error("Invalid hostname/port");

	if ((sock = p_socket_new(P_SOCKET_FAMILY_INET, P_SOCKET_TYPE_STREAM, P_SOCKET_PROTOCOL_TCP, nullptr)) == nullptr) {
		p_socket_address_free(addr);
		throw std::runtime_error("Could not create a TCP socket");
	}
	if (!p_socket_bind(sock, addr, FALSE, nullptr)) {
		p_socket_address_free(addr);
		p_socket_free(sock);

		throw std::runtime_error("Could not bind the socket to specified endpoint");
	}
	p_socket_address_free(addr);

	if (!p_socket_listen(sock, nullptr)) {
		p_socket_close(sock, nullptr);
		throw std::runtime_error("Could not start listening on the given port");
		
	}
	_running = true;
	_listenSocket = sock;
	_listenThread = std::thread([this] {
		listenWorker();
	});

}

void Server::stop() {
	if (!_running || !_listenSocket || p_socket_is_closed(_listenSocket))
		return;
	_running = false;

	p_socket_close(_listenSocket, nullptr);
	_listenThread.detach();
	auto clientsCopy = std::map<int, ConnectionBase*>(_clients);
	for (auto client : clientsCopy) {
		try {
			client.second->close();
		}
		catch (...) {}
	}
	_clients.clear();
}

void Server::writeToAll(const Serializable& msg) {
	for(const auto& con : _clients) {
		try {
			con.second->writeSync(msg);
		}catch(...){}
	}
}

void Server::join() {
	if (!_running)
		return;
	_listenThread.join();
}

Server::Server() {
	_listenSocket = nullptr;
}

Server::~Server() {
	stop();
	_observers.clear();
}

void Server::subscribe(ServerObserver* observer) {
	if (observer) {
		_observers.insert(observer);
		observer->setServer(this);
	}
}

void Server::unsubscribe(ServerObserver* observer) {
	_observers.erase(observer);
	if (observer) {
		observer->setServer(nullptr);
	}
}

void Server::deleteConnectionAsync(ConnectionBase* connection) {
	using namespace std::chrono_literals;

	std::thread thr([connection] {
		while (connection->isAlive() || connection->isReadingAsync()) {
			std::this_thread::sleep_for(1ms);
		}
		delete connection;
	});
	thr.detach();
}
