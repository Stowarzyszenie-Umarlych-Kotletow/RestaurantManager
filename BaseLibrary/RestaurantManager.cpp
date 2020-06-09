#include "RestaurantManager.h"

#include "C2S_Authorize.h"
#include "C2S_DeleteShift.h"
#include "C2S_GetShiftsByDay.h"
#include "C2S_GetWorkers.h"
#include "C2S_InsertShift.h"
#include "C2S_DeleteWorker.h"
#include "PingReply.h"
#include "Server.h"
#include "Ping.h"

#include "S2C_AuthorizeReply.h"
#include "S2C_DeleteShiftReply.h"
#include "S2C_DeleteWorkerReply.h"
#include "S2C_GetShiftsReply.h"
#include "S2C_GetWorkersReply.h"
#include "S2C_InsertShiftReply.h"
#include "S2C_InsertWorkerReply.h"
#include "S2C_ClientSync.h"


bool RestaurantManager::verifyPermission(ConnectionBase* connection, UserPermissions required) {
	const auto permissions = connection->getData().get<UserPermissions>("Permissions", UserPermissions::None);
	return (permissions & required) == required;
}


void RestaurantManager::handlePing(ConnectionBase* connection, const Ping& payload, size_t size) {
	connection->writeSync(PingReply());
}

void RestaurantManager::handleAuthorize(ConnectionBase* connection, const C2S_Authorize& payload, size_t size) {
	auto it = _accessTokens.find(payload.getToken());
	S2C_AuthorizeReply reply(payload.getRequestId());
	if (it == _accessTokens.end()) {
		reply.setErrorMsg("Invalid access token");
	}
	else {
		reply.setPermissions(it->second);
	}
	connection->getData().put("Permissions", reply.getPermissions());
	connection->writeSync(reply);
}

void RestaurantManager::handleGetShiftsByDay(ConnectionBase* connection, const C2S_GetShiftsByDay& payload,
                                             size_t size) {
	S2C_GetShiftsReply reply(payload.getRequestId(), payload.getDate());
	if (!verifyPermission(connection, UserPermissions::View)) {
		reply.setErrorMsg("Unauthorized");
	}
	else {
		auto it = _shiftsByDay.find(payload.getDate());
		if (it != _shiftsByDay.end()) {
			auto& shifts = reply.getShifts();
			
			for (auto id : it->second) {
				try {
					shifts.insert(_shifts[id]);
				}
				catch (std::exception&) {}
			}
		}
	}
	connection->writeSync(reply);

}

void RestaurantManager::handleInsertShift(ConnectionBase* connection, const C2S_InsertShift& payload, size_t size) {
	S2C_InsertShiftReply reply(payload.getRequestId(), payload.getShift());
	if (!verifyPermission(connection, UserPermissions::Insert)) {
		reply.setErrorMsg("Unauthorized");
	}
	else {
		try {
			auto& ret = insertShift(payload.getShift(), payload.isModifyExisting());
			reply.setShift(ret);
		}
		catch (std::exception& ex) {
			reply.setErrorMsg(ex.what());
		}
	}
	connection->writeSync(reply);

}

void RestaurantManager::handleDeleteShift(ConnectionBase* connection, const C2S_DeleteShift& payload, size_t size) {
	S2C_DeleteShiftReply reply(payload.getRequestId(), payload.getShiftId());
	if (!verifyPermission(connection, UserPermissions::Delete)) {
		reply.setErrorMsg("Unauthorized");
	}
	else {
		reply.setSuccess(deleteShift(payload.getShiftId()));
	}
	connection->writeSync(reply);

}

void RestaurantManager::handleGetWorkers(ConnectionBase* connection, const C2S_GetWorkers& payload, size_t size) {
	S2C_GetWorkersReply reply(payload.getRequestId());
	if (!verifyPermission(connection, UserPermissions::View)) {
		reply.setErrorMsg("Unauthorized");
	}
	else {
		reply.setWorkers(_workers);
	}
	connection->writeSync(reply);
}

void RestaurantManager::handleInsertWorker(ConnectionBase* connection, const C2S_InsertWorker& payload, size_t size) {
	S2C_InsertWorkerReply reply(payload.getRequestId(), payload.getWorker());
	if (!verifyPermission(connection, UserPermissions::Insert)) {
		reply.setErrorMsg("Unauthorized");
	}
	else {
		try {
			auto& ret = insertWorker(payload.getWorker(), payload.isModifyExisting());
			reply.setWorker(ret);
		}
		catch (std::exception& ex) {
			reply.setErrorMsg(ex.what());
		}
	}
	connection->writeSync(reply);
}

void RestaurantManager::handleDeleteWorker(ConnectionBase* connection, const C2S_DeleteWorker& payload, size_t size) {
	S2C_DeleteWorkerReply reply(payload.getRequestId(), payload.getWorkerId());
	if (!verifyPermission(connection, UserPermissions::Delete)) {
		reply.setErrorMsg("Unauthorized");
	}
	else {
		reply.setSuccess(deleteWorker(payload.getWorkerId()));
	}
	connection->writeSync(reply);
}

bool RestaurantManager::verifyShift(const Shift& shift) {
	std::lock_guard<std::recursive_mutex> guard(_lock);


	const auto start = shift.getStartTime();
	const auto end = shift.getEndTime();

	auto it = _shiftsByDay.find(start);

	if (it != _shiftsByDay.end()) {
		//check collisions
		for (const auto oShiftId : it->second) {
			if (oShiftId == shift.getId())
				continue;
			const auto oShift = _shifts[oShiftId];
			if (oShift.getJobName() != shift.getJobName())
				continue;
			const auto oStart = oShift.getStartTime();
			const auto oEnd = oShift.getEndTime();
			if (!((start < oStart && end <= oStart) || (start >= oEnd)))
				return false;
		}
	}
	return true;
}


Shift& RestaurantManager::insertShift(Shift shift, bool modify) {
	Shift* ref = nullptr;
	{
		std::lock_guard<std::recursive_mutex> guard(_lock);

		auto id = shift.getId();

		if (!modify) {
			shift.setId(0);
		}


		if (!verifyShift(shift)) {
			throw std::invalid_argument("Shift collides with existing ones");
		}

		if (modify) {
			if (!deleteShift(id)) {
				throw std::invalid_argument("Shift to be edited was not found");
			}
		}
		else {
			shift.setId(id = newIdentityId(_shifts));
		}
		ref = &(_shifts[id] = shift);
		_shiftsByDay[shift.getStartTime()].insert(id);
	}
	if (_server) {
		S2C_ClientSync sync;
		sync.getChangedShifts().insert(shift);
		_server->writeToAll(sync);
	}

	
	return *ref;
}

bool RestaurantManager::deleteShift(identity_t shiftId) {
	{
		std::lock_guard<std::recursive_mutex> guard(_lock);

		auto it = _shifts.find(shiftId);
		if (it == _shifts.end())
			return false;
		_shiftsByDay[it->second.getStartTime()].erase(shiftId);
		_shifts.erase(shiftId);
	}
	if (_server) {
		S2C_ClientSync sync;
		sync.getRemovedShifts().insert(shiftId);
		_server->writeToAll(sync);
	}
	return true;
}

bool RestaurantManager::deleteWorker(identity_t workerId) {
	{
		std::lock_guard<std::recursive_mutex> guard(_lock);

		const auto it = _workers.find(workerId);
		if (it == _workers.end())
			return false;

		for (auto& kv : _shifts) {
			auto& shift = kv.second;
			if (shift.getWorkerId() == workerId)
				shift.setWorkerId(0);
		}
		_workers.erase(workerId);
	}
	if (_server) {
		S2C_ClientSync sync;
		sync.getRemovedWorkers().insert(workerId);
		_server->writeToAll(sync);
	}
	return true;
}

std::ostream& RestaurantManager::serialize(std::ostream& dst) const {
	Serializable::serialize(dst);
	write_primitive<size_t>(dst, _shifts.size());
	for (const auto& kv : _shifts) {
		write_primitive(dst, kv.first);
		kv.second.serialize(dst);
	}
	write_primitive<size_t>(dst, _shiftsByDay.size());
	for (const auto& kv : _shiftsByDay) {
		kv.first.serialize(dst);
		write_set_primitive(dst, kv.second);
	}
	write_primitive<size_t>(dst, _workers.size());
	for (const auto& kv : _workers) {
		write_primitive(dst, kv.first);
		kv.second.serialize(dst);
	}
	write_primitive<size_t>(dst, _accessTokens.size());
	for (const auto& kv : _accessTokens) {
		write_string(dst, kv.first);
		write_primitive(dst, kv.second);
	}
	return dst;
}

std::istream& RestaurantManager::deserialize(std::istream& src) {
	Serializable::deserialize(src);
	_shifts.clear();
	_shiftsByDay.clear();
	_workers.clear();
	_accessTokens.clear();
	auto count = read_primitive<size_t>(src);
	for (size_t i = 0; i < count; i++) {
		auto id = read_primitive<identity_t>(src);
		auto shift = get_instance<Shift>(src);
		_shifts[id] = shift;
	}
	count = read_primitive<size_t>(src);
	for (size_t i = 0; i < count; i++) {
		auto date = get_instance<Date>(src);
		std::set<identity_t> set;
		read_set_primitive(src, set);
		_shiftsByDay[date] = set;
	}
	count = read_primitive<size_t>(src);
	for (size_t i = 0; i < count; i++) {
		auto id = read_primitive<identity_t>(src);
		auto worker = get_instance<ShiftWorker>(src);
		_workers[id] = worker;
	}
	count = read_primitive<size_t>(src);
	for (size_t i = 0; i < count; i++) {
		auto token = read_string(src);
		_accessTokens[token] = read_primitive<UserPermissions>(src);
	}
	return src;
}


ShiftWorker& RestaurantManager::insertWorker(ShiftWorker worker, bool modify) {
	ShiftWorker* ref = nullptr;
	{
		std::lock_guard<std::recursive_mutex> guard(_lock);

		identity_t id = worker.getId();


		if (modify) {
			if (_workers.find(id) == _workers.end()) {
				throw std::invalid_argument("Worker to be edited was not found");
			}
		}
		else {
			worker.setId(id = newIdentityId(_workers));
		}
		ref = &(_workers[id] = worker);
	}
	if (_server) {
		S2C_ClientSync sync;
		sync.getChangedWorkers().insert(worker);
		_server->writeToAll(sync);
	}
	return *ref;
}
