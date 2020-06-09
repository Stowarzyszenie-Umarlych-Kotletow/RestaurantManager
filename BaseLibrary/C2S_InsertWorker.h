#pragma once
#include "Serializable.h"
#include "binary.h"
#include "Shift.h"

#include <utility>

#include "ShiftWorker.h"
#include "TrackablePacket.h"
using namespace Serialization;
using namespace Binary;
/**
 * Client-to-server request to insert a ShiftWorker object
 * 
 */
class C2S_InsertWorker : public TrackablePacket {
protected:
	ShiftWorker _worker;
	bool _modifyExisting;
public:
	Type getType() const override {
		return Type::_C2S_InsertWorker;
	}

	/**
	 * Gets the @{code ShiftWorker}
	 * 
	 * @return ShiftWorker 
	 */
	virtual ShiftWorker getWorker() const {
		return _worker;
	}
	/**
	 * Gets the @{code ShiftWorker} by reference
	 * 
	 * @return ShiftWorker& 
	 */
	virtual ShiftWorker& getWorker() {
		return _worker;
	}
	/**
	 * Sets the @{code ShiftWorker}
	 * 
	 * @param shift 
	 */
	virtual void setWorker(const ShiftWorker& shift) {
		_worker = shift;
	}
	/**
	 * Returns whether this request should modify existing records
	 * 
	 * @return true edit operation
	 * @return false insert operation
	 */
	virtual bool isModifyExisting() const {
		return _modifyExisting;
	}
	/**
	 * Sets whether this request should modify existing records
	 * 
	 * @param modifyExisting whether to update existing records
	 */
	virtual void setModifyExisting(const bool modifyExisting) {
		_modifyExisting = modifyExisting;
	}

	C2S_InsertWorker() : C2S_InsertWorker(ShiftWorker()) {}
	/**
	 * Construct a new ShiftWorker insert request
	 * 
	 * @param shift the object to be inserted
	 * @param modifyExisting whether to modify existing records
	 */
	C2S_InsertWorker(ShiftWorker worker, bool modifyExisting = false) {
		_worker = std::move(worker);
		_modifyExisting = modifyExisting;
	}

	std::ostream& serialize(std::ostream& destination) const override {
		TrackablePacket::serialize(destination);
		_worker.serialize(destination);
		write_primitive(destination, _modifyExisting);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		TrackablePacket::deserialize(source);
		auto instance = new_instance<ShiftWorker>(source);
		instance->deserialize(source);
		_worker = *instance;
		read_primitive(source, &_modifyExisting);
		return source;
	}


	friend bool operator==(const C2S_InsertWorker& lhs, const C2S_InsertWorker& rhs) {
		return std::tie(static_cast<const TrackablePacket&>(lhs), lhs._worker, lhs._modifyExisting) == std::tie(
			static_cast<const TrackablePacket&>(rhs), rhs._worker, rhs._modifyExisting);
	}

	friend bool operator!=(const C2S_InsertWorker& lhs, const C2S_InsertWorker& rhs) {
		return !(lhs == rhs);
	}
};
