#pragma once
#include <set>
#include <utility>


#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
#include "Shift.h"
#include "ShiftWorker.h"
#include "TransactionReply.h"
/**
 * Server-to-client broadcast packet that updates the client state
 * 
 */
class S2C_ClientSync : public Serializable {
protected:
	std::set<identity_t> _removedWorkers;
	std::set<identity_t> _removedShifts;
	std::set<ShiftWorker> _changedWorkers;
	std::set<Shift> _changedShifts;
public:
	Type getType() const override {
		return Type::_S2C_ClientSync;
	}
	/**
	 * Gets the removed worker ids
	 * 
	 * @return std::set<identity_t> 
	 */
	std::set<identity_t> getRemovedWorkers() const { return _removedWorkers; }
	/**
	 * Gets the removed shift ids
	 * 
	 * @return std::set<identity_t> 
	 */
	std::set<identity_t> getRemovedShifts() const { return _removedShifts; }
	/**
	 * Gets the removed worker ids by reference
	 * 
	 * @return std::set<identity_t>& 
	 */
	std::set<identity_t>& getRemovedWorkers() { return _removedWorkers; }
	/**
	 * Gets the removed shift ids by reference
	 * 
	 * @return std::set<identity_t>& 
	 */
	std::set<identity_t>& getRemovedShifts() { return _removedShifts; }
	/**
	 * Gets the set of changed ShiftWorker objects
	 * 
	 * @return std::set<ShiftWorker> 
	 */
	std::set<ShiftWorker> getChangedWorkers() const { return _changedWorkers; }
	/**
	 * Gets the set of changed Shift objects
	 * 
	 * @return std::set<Shift> 
	 */
	std::set<Shift> getChangedShifts() const { return _changedShifts; }
	/**
	 * Gets the set of changed ShiftWorker objects by reference
	 * 
	 * @return std::set<ShiftWorker>& 
	 */
	std::set<ShiftWorker>& getChangedWorkers() { return _changedWorkers; }
	/**
	 * Gets the set of changed Shift objects by reference
	 * 
	 * @return std::set<Shift>& 
	 */
	std::set<Shift>& getChangedShifts() { return _changedShifts; }

	std::ostream& serialize(std::ostream& dst) const override {
		Serializable::serialize(dst);
		write_set_primitive<identity_t>(dst, _removedWorkers);
		write_set_primitive<identity_t>(dst, _removedShifts);
		write_set_typed<Shift>(dst, _changedShifts);
		write_set_typed<ShiftWorker>(dst, _changedWorkers);
		return dst;
	}

	std::istream& deserialize(std::istream& src) override {
		Serializable::deserialize(src);
		read_set_primitive<identity_t>(src, _removedWorkers);
		read_set_primitive<identity_t>(src, _removedShifts);
		read_set(src, _changedShifts, get_type_deserializer<Shift>());
		read_set(src, _changedWorkers, get_type_deserializer<ShiftWorker>());
		return src;
	}


	friend bool operator==(const S2C_ClientSync& lhs, const S2C_ClientSync& rhs) {
		return std::tie(static_cast<const Serializable&>(lhs), lhs._removedWorkers, lhs._removedShifts,
		                lhs._changedWorkers, lhs._changedShifts) == std::tie(
			static_cast<const Serializable&>(rhs), rhs._removedWorkers, rhs._removedShifts, rhs._changedWorkers,
			rhs._changedShifts);
	}

	friend bool operator!=(const S2C_ClientSync& lhs, const S2C_ClientSync& rhs) {
		return !(lhs == rhs);
	}
};
