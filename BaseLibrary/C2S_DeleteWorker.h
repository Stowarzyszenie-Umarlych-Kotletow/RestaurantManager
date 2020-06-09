#pragma once
#include "Serializable.h"
#include "binary.h"
#include "Shift.h"
#include "TrackablePacket.h"
using namespace Serialization;
using namespace Binary;
/**
 * Client-to-server packet that deletes a ShiftWorker by its id
 * 
 */
class C2S_DeleteWorker : public TrackablePacket {
protected:
	identity_t _workerId;
public:
	Type getType() const override {
		return Type::_C2S_DeleteWorker;
	}

	/**
	 * Gets the target ShiftWorker id
	 * 
	 * @return identity_t 
	 */
	virtual identity_t getWorkerId() const {
		return _workerId;
	}
	/**
	 * Sets the target ShiftWorker id
	 * 
	 * @param shiftId 
	 */
	virtual void setWorkerId(const identity_t shiftId) {
		_workerId = shiftId;
	}

	C2S_DeleteWorker() : C2S_DeleteWorker(0) {}
	/**
	 * Construct a new request to delete a ShfitWorker by its id
	 * 
	 * @param workerId target ShiftWorker id
	 */
	C2S_DeleteWorker(identity_t workerId) {
		_workerId = workerId;
	}

	std::ostream& serialize(std::ostream& destination) const override {
		TrackablePacket::serialize(destination);
		write_primitive(destination, _workerId);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		TrackablePacket::deserialize(source);
		read_primitive(source, &_workerId);
		return source;
	}


	friend bool operator==(const C2S_DeleteWorker& lhs, const C2S_DeleteWorker& rhs) {
		return std::tie(static_cast<const TrackablePacket&>(lhs), lhs._workerId) == std::tie(
			static_cast<const TrackablePacket&>(rhs), rhs._workerId);
	}

	friend bool operator!=(const C2S_DeleteWorker& lhs, const C2S_DeleteWorker& rhs) {
		return !(lhs == rhs);
	}
};
