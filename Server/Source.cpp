#include <csignal>
#include <fstream>
#include "Connection.h"
#include "BaseLibrary.h"
#include "C2S_Authorize.h"
#include "Server.h"
#include "RestaurantManager.h"
#include "S2C_ClientSync.h"


const std::string HOST = "127.0.0.1";
const int PORT = 1337;
const std::string STORAGE_PATH = "store.bin";

std::map<std::string, UserPermissions> TOKENS = {
	{"arbuz", UserPermissions::NORMAL_USER},
	{"melon", UserPermissions::SUPER_USER},
};

/**
 * Logging handlers
 */
class ConnectionEventHandler : public ServerObserver {
protected:
	/**
	 * Returns a short string of permissions granted for the provided connection
	 */
	std::string strPermissions(ConnectionBase* connection) {
		return getPermissionsString(connection->getData().get<UserPermissions>("Permissions", 
			UserPermissions::None));
	}
public:
	ConnectionEventHandler() = default;

	void onConnected(ConnectionBase* connection) override {
		printf("Connection #%d (%s) connected\n", connection->getId(), strPermissions(connection).c_str());
	}

	void onDisconnected(ConnectionBase* connection, std::exception exception) override {
		printf("Connection #%d (%s) disconnected (%s)\n", connection->getId(), 
			strPermissions(connection).c_str(), exception.what());
	}

	void onPayloadSent(ConnectionBase* connection, const Serializable& payload, size_t size) override {
		printf("Connection #%d (%s): sent payload of type `%s` (%ub)\n", connection->getId(),
			strPermissions(connection).c_str(), get_type_name(payload.getType()).c_str(), size);
		// print all transaction replies
		if (is_assignable(Type::_TransactionReply, payload.getType())) {
			const auto& reply = dynamic_cast<const TransactionReply&>(payload);
			printf("\tReply to R%d, success=%d\n", reply.getRequestId(), static_cast<int>(reply.isSuccess()));
			if (!reply.isSuccess()) {
				printf("\tError: %s\n", reply.getErrorMsg().c_str());
			}
		}
	}

};

Server* server;
ConnectionEventHandler* handler;
RestaurantManager* mgr;

/**
 * Loads the RestaurantManager from the specified file
 */
bool loadManager(const std::string& path) {
	std::ifstream f(path);
	if (!f.good())
		return false;
	std::cout << "Importing manager from storage... ";
	try {
		mgr->deserialize(f);
		std::cout << "OK" << std::endl;
	} catch(std::exception& ex) {
		std::cout << ex.what() << std::endl;
		return false;
	}
	return true;
}
/**
 * Saves the RestaurantManager into the specified file
 */
void saveManager(const std::string& path) {
	std::ofstream f(path);
	if (!f.good())
		return;
	std::cout << "Saving manager instance... ";
	try {
		mgr->serialize(f);
		std::cout << "OK" << std::endl;
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
}

void onExit() {
	static bool exited = false;
	if (exited) return;
	exited = true;
	saveManager(STORAGE_PATH);
	
	std::cout << "Stopping the server... ";
	server->stop();
	std::cout << "OK" << std::endl;

	
	delete mgr;
	delete server;
	delete handler;
	ShutdownBaseLibrary();
}


BOOL WINAPI consoleHandler(DWORD signal) {

	if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) {
		onExit();
	}

	return FALSE;
}


int main() {
	InitializeBaseLibrary();

	if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
		std::cout << "ERROR: Could not set console handler";;
		return 1;
	}
	
	handler = new ConnectionEventHandler;
	server = new Server;
	mgr = new RestaurantManager(server);
	
	if (!loadManager(STORAGE_PATH)) {
		std::cout << "Could not find an existing manager instance" << std::endl;
	}

	auto& tokens = mgr->getAccessTokens();

	for(const auto& kv : TOKENS) {
		tokens[kv.first] = kv.second;
	}
	
	server->subscribe(mgr);
	server->subscribe(handler);
	try {
		server->start(HOST, PORT);
	} catch(std::exception& ex) {
		std::cout << ex.what() << std::endl;
		onExit();
		return 0;
	}
	std::cout << "Server is listening on " << HOST << ":" << PORT << std::endl;
	server->join();


	return 0;
}
