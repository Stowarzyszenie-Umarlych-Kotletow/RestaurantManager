#pragma once
#include "Serializable.h"
#include "binary.h"
#include "Shift.h"

#include <utility>
#include "TrackablePacket.h"
using namespace Serialization;
using namespace Binary;
/**
 * Client-to-server packet that deletes a Shift by its id
 * 
 */
class C2S_DeleteShift : public TrackablePacket {
protected:
	identity_t _shiftId;
public:
	Type getType() const override {
		return Type::_C2S_DeleteShift;
	}

	/**
	 * Gets the target Shift id 
	 * 
	 * @return identity_t 
	 */
	virtual identity_t getShiftId() const {
		return _shiftId;
	}
	/**
	 * Sets the target Shift id
	 * 
	 * @param shiftId 
	 */
	virtual void setShiftId(const identity_t shiftId) {
		_shiftId = shiftId;
	}

	C2S_DeleteShift() : C2S_DeleteShift(0) {}
	/**
	 * Construct a new request to delete the given Shift object by its id
	 * 
	 * @param shiftId target Shift id
	 */
	C2S_DeleteShift(identity_t shiftId) {
		_shiftId = shiftId;
	}

	std::ostream& serialize(std::ostream& destination) const override {
		TrackablePacket::serialize(destination);
		write_primitive(destination, _shiftId);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		TrackablePacket::deserialize(source);
		read_primitive(source, &_shiftId);
		return source;
	}


	friend bool operator==(const C2S_DeleteShift& lhs, const C2S_DeleteShift& rhs) {
		return std::tie(static_cast<const TrackablePacket&>(lhs), lhs._shiftId) == std::tie(
			static_cast<const TrackablePacket&>(rhs), rhs._shiftId);
	}

	friend bool operator!=(const C2S_DeleteShift& lhs, const C2S_DeleteShift& rhs) {
		return !(lhs == rhs);
	}
};
