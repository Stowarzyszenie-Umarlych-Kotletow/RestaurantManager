#pragma once
#include <utility>
#include "Connection.h"
#include "Date.h"
#include "Shift.h"
#include "ShiftWorker.h"
#include "S2C_AuthorizeReply.h"
#include "S2C_DeleteShiftReply.h"
#include "S2C_GetShiftsReply.h"
#include "S2C_GetWorkersReply.h"
#include "S2C_InsertShiftReply.h"
#include "S2C_InsertWorkerReply.h"
#include "S2C_DeleteWorkerReply.h"
#include "S2C_ClientSync.h"

/**
 * Client interface to access resources provided by the remote RestaurantManager
 */
class RestaurantClient : public ConnectionObserver {
protected:
	Connection _connection;
	std::string _host;
	int _port;
	std::string _token;
	std::map<identity_t, ShiftWorker> _workers;
	std::map<identity_t, Shift> _shifts;
	std::map<Date, std::set<identity_t>> _shiftsByDay;
	UserPermissions _permissions;

	/**
	 * Sends a request to the server while ensuring the internal Connection is alive
	 */
	int writeRequest(TrackablePacket& payload);


public:
	/**
	 * Gets local ShiftWorker objects
	 */
	std::map<identity_t, ShiftWorker>& getWorkers() {
		return _workers;
	}

	/**
	 * Gets a local ShiftWorker object by its id
	 */
	ShiftWorker& getWorker(identity_t id);

	/**
	 * Inserts a Shift into the local database
	 */
	void insertShift(const Shift& shift);

	/**
	 * Deletes a Shift from the local database by its id
	 */
	void deleteShift(identity_t id);

	/**
	 * Deletes a ShiftWorker from the local database by its id
	 */
	void deleteWorker(identity_t id);

	/**
	 * Gets a Shift from the local database by its id
	 */
	Shift& getShift(identity_t id);

	/**
	 * Gets local Shift objects
	 */
	std::map<identity_t, Shift>& getShifts() {
		return _shifts;
	}

	/**
	 * Gets all local Shift objects by Date
	 */
	std::set<std::reference_wrapper<Shift>> getShifts(Date day);


protected:
	/**
	 * Event handlers
	 */
	void onAuthorize(ConnectionBase* connection, const S2C_AuthorizeReply& payload, size_t size);
	void onDeleteShift(ConnectionBase* connection, const S2C_DeleteShiftReply& payload, size_t size);
	void onGetShiftsByDay(ConnectionBase* connection, const S2C_GetShiftsReply& payload, size_t size);
	void onGetWorkers(ConnectionBase* connection, const S2C_GetWorkersReply& payload, size_t size);
	void onInsertShift(ConnectionBase* connection, const S2C_InsertShiftReply& payload, size_t size);
	void onInsertWorker(ConnectionBase* connection, const S2C_InsertWorkerReply& payload, size_t size);
	void onDeleteWorker(ConnectionBase* connection, const S2C_DeleteWorkerReply& payload, size_t size);
	void onSync(ConnectionBase* connection, const S2C_ClientSync& payload, size_t size);
	void onConnected(ConnectionBase* connection) override;

public:
	RestaurantClient() : RestaurantClient("", 0) { }

	RestaurantClient(std::string host, int port, std::string token = "") {
		setEndpoint(host, port);
		_token = std::move(token);
		_connection.subscribe(this);
		addHandler(&RestaurantClient::onAuthorize);
		addHandler(&RestaurantClient::onDeleteShift);
		addHandler(&RestaurantClient::onGetShiftsByDay);
		addHandler(&RestaurantClient::onGetWorkers);
		addHandler(&RestaurantClient::onInsertShift);
		addHandler(&RestaurantClient::onInsertWorker);
		addHandler(&RestaurantClient::onDeleteWorker);
		addHandler(&RestaurantClient::onSync);
	}

	/**
	 * Sets the endpoint for the remote server
	 */
	void setEndpoint(std::string host, int port) {
		_host = std::move(host);
		_port = port;
	}

	/**
	 * Returns the internal Connection object by const reference
	 */
	const Connection& getConnection() const {
		return _connection;
	}

	/**
	 * Returns the internal Connection object by reference
	 */
	Connection& getConnection() {
		return _connection;
	}

	/**
	 * Gets the granted UserPermissions
	 */
	UserPermissions getPermissions() const {
		return _permissions;
	}

	/**
	 * Reconnects to the remote server if necessary
	 */
	void ensureConnected() {
		if (!isAlive())
			connect();
	}

	/**
	 * Returns the current authorization token
	 */
	std::string getToken() const {
		return _token;
	}

	/**
	 * Sets the authorization token
	 */
	void setToken(const std::string& token) {
		_token = token;
	}

	/**
	 * Connects to the remote endpoint
	 */
	void connect();

	/**
	 * Closes the internal Connection object
	 */
	void close() {
		_connection.close();
	}

	/**
	 * Returns whether the connection to remote server is active
	 */
	bool isAlive() const {
		return _connection.isAlive();
	}

	/*
	 * Sends an authorize request to the server
	 * @return unique request id
	 */
	int authorize();
	/**
	 * Sends an authorize request to the server with the specified token
	 * @return unique request id
	 */
	int authorize(std::string newToken);
	/**
	 * Sends a request to insert the provided Shift object
	 * @param shift Shift object
	 * @param modify whether to modify an existing object
	 * @return unique request id
	 */
	int queryInsertShift(const Shift& shift, bool modify = false);

	/**
	 * Sends a request to update the provided Shift object
	 * @return unique request id
	 */
	int queryUpdateShift(const Shift& shift) {
		return queryInsertShift(shift, true);
	}

	/**
	 * Sends a request to delete a Shift by its id
	 * @return unique request id
	 */
	int queryDeleteShift(identity_t shiftId);
	/**
	 * Sends a request to list all Shift objects in the specified Date
	 * @return unique request id
	 */
	int queryShiftsByDay(const Date& day);
	/**
	 * Sends a request to list all ShiftWorker objects
	 * @return unique request id
	 */
	int queryWorkers();
	/**
	 * Sends a request to insert the provided ShiftWorker object
	 * @param worker ShiftWorker object
	 * @param modify whether to update an existing object
	 * @return unique request id
	 */
	int queryInsertWorker(const ShiftWorker& worker, bool modify = false);

	/**
	 * Sends a request to update the provided ShiftWorker object
	 * @param worker ShiftWorker object
	 * @return unique request id
	 */
	int queryUpdateWorker(const ShiftWorker& worker) {
		return queryInsertWorker(worker, true);
	}

	/**
	 * Sends a request to delete a ShiftWorker object by its id
	 * @return unique request id
	 */
	int queryDeleteWorker(identity_t workerId);
};
