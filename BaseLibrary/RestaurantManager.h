#pragma once
#include <set>


#include "C2S_Authorize.h"
#include "C2S_DeleteShift.h"
#include "C2S_GetShiftsByDay.h"
#include "C2S_GetWorkers.h"
#include "C2S_InsertShift.h"
#include "C2S_InsertWorker.h"
#include "C2S_DeleteWorker.h"
#include "Ping.h"
#include "PingReply.h"
#include "Serializable.h"
#include "Server.h"
#include "Shift.h"
#include "ShiftWorker.h"
#include "TransactionReply.h"
#include "UserPermissions.h"
using namespace Serialization;

/**
 * Class that binds to a Server and provides the RestaurantManager service
 * 
 */
class RestaurantManager : Serializable, public ServerObserver {
protected:
	Server* _server;
	
	std::recursive_mutex _lock;
	std::map<identity_t, Shift> _shifts;
	std::map<Date, std::set<identity_t>> _shiftsByDay;
	std::map<identity_t, ShiftWorker> _workers;
	std::map<std::string, UserPermissions> _accessTokens;

	void onPayloadReceived(ConnectionBase* connection, const Serializable& payload, size_t size) override {
		std::lock_guard<std::recursive_mutex> guard(_lock);
		// synchronize payload handlers
		ServerObserver::onPayloadReceived(connection, payload, size);
	}



public:
	Type getType() const override {
		return Type::_RestaurantManager;
	}


	RestaurantManager(Server* server) : ServerObserver() {
		_server = server;
		// member handlers
		addHandler(&RestaurantManager::handlePing);
		addHandler(&RestaurantManager::handleAuthorize);
		addHandler(&RestaurantManager::handleDeleteShift);
		addHandler(&RestaurantManager::handleGetShiftsByDay);
		addHandler(&RestaurantManager::handleGetWorkers);
		addHandler(&RestaurantManager::handleInsertShift);
		addHandler(&RestaurantManager::handleInsertWorker);
		addHandler(&RestaurantManager::handleDeleteWorker);
	}

	void handlePing(ConnectionBase* connection, const Ping& payload, size_t size);
	void handleAuthorize(ConnectionBase* connection, const C2S_Authorize& payload, size_t size);
	void handleGetShiftsByDay(ConnectionBase* connection, const C2S_GetShiftsByDay& payload, size_t size);
	void handleInsertShift(ConnectionBase* connection, const C2S_InsertShift& payload, size_t size);
	void handleDeleteShift(ConnectionBase* connection, const C2S_DeleteShift& payload, size_t size);
	void handleGetWorkers(ConnectionBase* connection, const C2S_GetWorkers& payload, size_t size);
	void handleInsertWorker(ConnectionBase* connection, const C2S_InsertWorker& payload, size_t size);
	void handleDeleteWorker(ConnectionBase* connection, const C2S_DeleteWorker& payload, size_t size);
	
	/**
	 * Checks the permissions of the connected Connection
	 * 
	 * @param connection client
	 * @param required required permissions
	 * @return whether the client has all the required permissions
	 */
	bool verifyPermission(ConnectionBase* connection, UserPermissions required);
	/**
	 * Generates a new identity id for the given collection
	 * 
	 * @tparam T collection type
	 * @param map collection
	 * @return identity_t new unique identity id
	 */
	template <typename T>
	static identity_t newIdentityId(const std::map<identity_t, T>& map) {
		identity_t id = 0;
		if (!map.empty())
			id = (--map.end())->first;
		return id + 1;
	}
	/**
	 * Gets the Shift objects by reference
	 * 
	 * @return std::map<identity_t, Shift>& 
	 */
	virtual std::map<identity_t, Shift>& getShifts() {
		return _shifts;
	}
	/**
	 * Gets the Shift objects by day by reference
	 * 
	 * @return std::map<Date, std::set<identity_t>>& 
	 */
	virtual std::map<Date, std::set<identity_t>>& getShiftsByDay() {
		return _shiftsByDay;
	}
	/**
	 * Gets the ShiftWorker objects by reference
	 * 
	 * @return std::map<identity_t, ShiftWorker>& 
	 */
	virtual std::map<identity_t, ShiftWorker>& getWorkers() {
		return _workers;
	}
	/**
	 * Gets the access tokens by reference
	 * 
	 * @return std::map<std::string, UserPermissions>& 
	 */
	virtual std::map<std::string, UserPermissions>& getAccessTokens() {
		return _accessTokens;
	}
	/**
	 * Insert a Shift into the database
	 * 
	 * @param shift object to be inserted
	 * @param modify whether to modify an existing record
	 * @return Shift& reference to the inserted object
	 */
	virtual Shift& insertShift(Shift shift, bool modify = false);
	/**
	 * Deletes a Shift from the database
	 * 
	 * @param shiftId id of the Shift
	 * @return whether an object was removed
	 */
	virtual bool deleteShift(identity_t shiftId);
	/**
	 * Checks if the Shift does not collide with others
	 * 
	 * @param shift object to be queried
	 * @return whether the shift can be inserted
	 */
	virtual bool verifyShift(const Shift& shift);
	/**
	 * Insert a ShiftWorker into the database
	 * 
	 * @param worker object to be inserted
	 * @param modify whether to modify an existing record
	 * @return ShiftWorker& reference to the inserted object
	 */
	virtual ShiftWorker& insertWorker(ShiftWorker worker, bool modify = false);
	/**
	 * Deletes a ShiftWorker from the database
	 * 
	 * @param workerId id of the ShiftWorker
	 * @return whether an object was removed
	 */
	virtual bool deleteWorker(identity_t workerId);

	std::ostream& serialize(std::ostream& dst) const override;

	std::istream& deserialize(std::istream& src) override;
};
