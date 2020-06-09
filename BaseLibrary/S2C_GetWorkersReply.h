#pragma once
#include <utility>


#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
#include "ShiftWorker.h"
#include "TransactionReply.h"
using namespace Serialization;
using namespace Binary;
/**
 * Server-to-client response that lists all ShiftWorker objects in the database
 * 
 */
class S2C_GetWorkersReply : public TransactionReply {
protected:
	std::map<identity_t, ShiftWorker> _workers;
public:
	Type getType() const override {
		return Type::_S2C_GetWorkersReply;
	}

	S2C_GetWorkersReply() : S2C_GetWorkersReply(0) {}
	/**
	 * Construct a new response that lists all ShiftWorkers
	 * 
	 * @param requestId request id
	 */
	S2C_GetWorkersReply(int requestId)
		: TransactionReply(requestId) {}
	/**
	 * Gets a copy of all ShiftWorkers
	 * 
	 * @return std::map<identity_t, ShiftWorker> copy of all ShiftWorkers
	 */
	std::map<identity_t, ShiftWorker> getWorkers() const {
		return _workers;
	}
	/**
	 * Gets all ShiftWorkers by reference
	 * 
	 * @return std::map<identity_t, ShiftWorker>& reference to all ShiftWorkers
	 */
	std::map<identity_t, ShiftWorker>& getWorkers() {
		return _workers;
	}
	/**
	 * Sets the set of ShiftWorkers
	 * 
	 * @param workers set of ShiftWorkers
	 */
	void setWorkers(std::map<identity_t, ShiftWorker> workers) {
		_workers = std::move(workers);
	}

	std::ostream& serialize(std::ostream& destination) const override {
		TransactionReply::serialize(destination);
		write_primitive<size_t>(destination, _workers.size());
		for (const auto& kv : _workers) {
			write_primitive(destination, kv.first);
			kv.second.serialize(destination);
		}
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		TransactionReply::deserialize(source);
		_workers.clear();
		size_t count;
		read_primitive(source, &count);
		for (size_t i = 0; i < count; i++) {
			identity_t id;
			read_primitive(source, &id);
			_workers[id] = get_instance<ShiftWorker>(source);
		}
		return source;
	}


	friend bool operator==(const S2C_GetWorkersReply& lhs, const S2C_GetWorkersReply& rhs) {
		return std::tie(static_cast<const TransactionReply&>(lhs), lhs._workers) == std::tie(
			static_cast<const TransactionReply&>(rhs), rhs._workers);
	}

	friend bool operator!=(const S2C_GetWorkersReply& lhs, const S2C_GetWorkersReply& rhs) {
		return !(lhs == rhs);
	}
};
