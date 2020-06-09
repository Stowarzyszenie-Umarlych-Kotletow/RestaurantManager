#include "RestaurantClient.h"

#include "C2S_Authorize.h"
#include "C2S_DeleteShift.h"
#include "C2S_DeleteWorker.h"
#include "C2S_GetShiftsByDay.h"
#include "C2S_GetWorkers.h"
#include "C2S_InsertShift.h"
#include "C2S_InsertWorker.h"
#include "S2C_DeleteWorkerReply.h"

int RestaurantClient::authorize() {
	C2S_Authorize request(_token);
	return writeRequest(request);
}

int RestaurantClient::authorize(std::string newToken) {
	_token = std::move(newToken);
	return authorize();
}

void RestaurantClient::connect() {
	_connection.connect(_host, _port);
}

int RestaurantClient::queryDeleteShift(identity_t shiftId) {
	C2S_DeleteShift request(shiftId);
	return writeRequest(request);
}

int RestaurantClient::queryShiftsByDay(const Date& day) {
	C2S_GetShiftsByDay request(day);
	return writeRequest(request);
}

int RestaurantClient::queryWorkers() {
	C2S_GetWorkers request;
	return writeRequest(request);
}

int RestaurantClient::queryInsertShift(const Shift& shift, bool modify) {
	C2S_InsertShift request(shift, modify);
	return writeRequest(request);
}

int RestaurantClient::queryInsertWorker(const ShiftWorker& worker, bool modify) {
	C2S_InsertWorker request(worker, modify);
	return writeRequest(request);
}

int RestaurantClient::queryDeleteWorker(identity_t workerId) {
	C2S_DeleteWorker request(workerId);
	return writeRequest(request);
}


int RestaurantClient::writeRequest(TrackablePacket& payload) {
	ensureConnected();
	return _connection.writeRequestSync(payload);
}

ShiftWorker& RestaurantClient::getWorker(identity_t id) {
	auto it = _workers.find(id);
	if (it == _workers.end()) {
		throw std::invalid_argument("Not Found");
	}
	return it->second;
}

void RestaurantClient::insertShift(const Shift& shift) {
	auto it = _shifts.find(shift.getId());
	if (it != _shifts.end()) {
		deleteShift(shift.getId());
	}
	_shifts[shift.getId()] = shift;
	_shiftsByDay[shift.getStartTime()].insert(shift.getId());
}

void RestaurantClient::deleteShift(identity_t id) {
	auto it = _shifts.find(id);
	if (it != _shifts.end()) {
		const auto& shift = it->second;
		_shiftsByDay[shift.getStartTime()].erase(shift.getId());
		_shifts.erase(shift.getId());
	}
}

void RestaurantClient::deleteWorker(identity_t id) {
	auto it = _workers.find(id);
	if (it != _workers.end()) {
		_workers.erase(it);
		for (auto& kv : _shifts) {
			auto& shift = kv.second;
			if (shift.getWorkerId() == id)
				shift.setWorkerId(0);
		}
	}
}

Shift& RestaurantClient::getShift(identity_t id) {
	auto it = _shifts.find(id);
	if (it == _shifts.end()) {
		throw std::invalid_argument("Not Found");
	}
	return it->second;
}

std::set<std::reference_wrapper<Shift>> RestaurantClient::getShifts(Date day) {
	std::set<std::reference_wrapper<Shift>> temp;
	auto it = _shiftsByDay.find(day);
	if (it == _shiftsByDay.end()) {
		return temp;
	}

	for (auto& kv : it->second) {
		temp.insert(getShift(kv));
	}
	return temp;
}

void RestaurantClient::onAuthorize(ConnectionBase* connection, const S2C_AuthorizeReply& payload, size_t size) {
	_permissions = payload.getPermissions();
}

void RestaurantClient::onDeleteShift(ConnectionBase* connection, const S2C_DeleteShiftReply& payload, size_t size) {
	if (payload.isSuccess()) {
		deleteShift(payload.getId());
	}
}

void RestaurantClient::onGetShiftsByDay(ConnectionBase* connection, const S2C_GetShiftsReply& payload, size_t size) {
	auto shifts = payload.getShifts();
	for (const auto& shift : shifts) {
		insertShift(shift);
	}
}

void RestaurantClient::onGetWorkers(ConnectionBase* connection, const S2C_GetWorkersReply& payload, size_t size) {
	for (auto& kv : payload.getWorkers()) {
		_workers[kv.first] = kv.second;
	}
}

void RestaurantClient::onInsertShift(ConnectionBase* connection, const S2C_InsertShiftReply& payload, size_t size) {
	if (payload.isSuccess()) {
		insertShift(payload.getShift());
	}
}

void RestaurantClient::onInsertWorker(ConnectionBase* connection, const S2C_InsertWorkerReply& payload, size_t size) {
	if (payload.isSuccess()) {
		_workers[payload.getWorker().getId()] = payload.getWorker();
	}
}

void RestaurantClient::onDeleteWorker(ConnectionBase* connection, const S2C_DeleteWorkerReply& payload, size_t size) {
	if (payload.isSuccess()) {
		_workers.erase(payload.getId());
		for (auto& kv : _shifts) {
			auto& shift = kv.second;
			if (shift.getWorkerId() == payload.getId())
				shift.setWorkerId(0);
		}
	}
}

void RestaurantClient::onSync(ConnectionBase* connection, const S2C_ClientSync& payload, size_t size) {
	for (auto id : payload.getRemovedShifts()) {
		deleteShift(id);
	}
	for (auto id : payload.getRemovedWorkers()) {
		deleteWorker(id);
	}
	for (const auto& worker : payload.getChangedWorkers()) {
		_workers[worker.getId()] = worker;
	}
	for (const auto& shift : payload.getChangedShifts()) {
		insertShift(shift);
	}
}

void RestaurantClient::onConnected(ConnectionBase* connection) {
	ConnectionObserver::onConnected(connection);
	_connection.setReadingAsync(true);
	authorize();
}
