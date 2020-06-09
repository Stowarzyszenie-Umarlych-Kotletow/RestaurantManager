#pragma once
#include "Serializable.h"
#include "binary.h"
#include "Shift.h"

#include <utility>
#include "TrackablePacket.h"
using namespace Serialization;
using namespace Binary;
/**
 * Client-to-server request to insert the given Shift object
 * 
 */
class C2S_InsertShift : public TrackablePacket {
protected:
	Shift _shift;
	bool _modifyExisting;
public:
	Type getType() const override {
		return Type::_C2S_InsertShift;
	}

	/**
	 * Gets the Shift object
	 * 
	 * @return Shift 
	 */
	virtual Shift getShift() const {
		return _shift;
	}
	/**
	 * Gets the Shift object by reference
	 * 
	 * @return Shift& 
	 */
	virtual Shift& getShift() {
		return _shift;
	}
	/**
	 * Sets the Shift object
	 * 
	 * @param shift new Shift object
	 */
	virtual void setShift(const Shift& shift) {
		_shift = shift;
	}
	/**
	 * Returns whether the request should modify the existing records
	 * 
	 * @return true edit the Shift
	 * @return false insert the Shift
	 */
	virtual bool isModifyExisting() const {
		return _modifyExisting;
	}
	/**
	 * Sets whether the request should modify the existing records
	 * 
	 * @param modifyExisting 
	 */
	virtual void setModifyExisting(const bool modifyExisting) {
		_modifyExisting = modifyExisting;
	}

	C2S_InsertShift() : C2S_InsertShift(Shift()) {}
	/**
	 * Construct a new insert request
	 * 
	 * @param shift object to be inserted
	 * @param modifyExisting whether to update existing records
	 */
	C2S_InsertShift(Shift shift, bool modifyExisting = false) {
		_shift = std::move(shift);
		_modifyExisting = modifyExisting;
	}

	std::ostream& serialize(std::ostream& destination) const override {
		TrackablePacket::serialize(destination);
		_shift.serialize(destination);
		write_primitive(destination, _modifyExisting);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		TrackablePacket::deserialize(source);
		auto instance = new_instance<Shift>(source);
		instance->deserialize(source);
		_shift = *instance;
		read_primitive(source, &_modifyExisting);
		return source;
	}


	friend bool operator==(const C2S_InsertShift& lhs, const C2S_InsertShift& rhs) {
		return std::tie(static_cast<const TrackablePacket&>(lhs), lhs._shift, lhs._modifyExisting) == std::tie(
			static_cast<const TrackablePacket&>(rhs), rhs._shift, rhs._modifyExisting);
	}

	friend bool operator!=(const C2S_InsertShift& lhs, const C2S_InsertShift& rhs) {
		return !(lhs == rhs);
	}
};
