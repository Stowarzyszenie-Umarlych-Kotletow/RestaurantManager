#pragma once
#include <utility>


#include "Serializable.h"
#include "binary.h"
#include "TrackablePacket.h"
using namespace Serialization;
using namespace Binary;
/**
 * Client-to-server request to list all Shift objects in a given date
 * 
 */
class C2S_GetShiftsByDay : public TrackablePacket {
protected:
	Date _date;
public:
	Type getType() const override {
		return Type::_C2S_GetShiftsByDay;
	}

	C2S_GetShiftsByDay() : C2S_GetShiftsByDay(Date()) {}
	/**
	 * Construct a new request to list shifts by date
	 * 
	 * @param date target date
	 */
	C2S_GetShiftsByDay(Date date) {
		_date = std::move(date);
	}
	/**
	 * Gets the target date by referece 
	 * 
	 * @return Date& target date
	 */
	Date& getDate() {
		return _date;
	}
	/**
	 * Gets the target date
	 * 
	 * @return const Date& 
	 */
	const Date& getDate() const {
		return _date;
	};
	/**
	 * Sets the target date
	 * 
	 * @param date 
	 */
	void setDate(Date date) {
		_date = std::move(date);
	}


	std::ostream& serialize(std::ostream& destination) const override {
		TrackablePacket::serialize(destination);
		_date.serialize(destination);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		TrackablePacket::deserialize(source);
		auto date = new_instance<Date>(source);
		date->deserialize(source);
		_date = *date;
		return source;
	}


	friend bool operator==(const C2S_GetShiftsByDay& lhs, const C2S_GetShiftsByDay& rhs) {
		return std::tie(static_cast<const TrackablePacket&>(lhs), lhs._date) == std::tie(
			static_cast<const TrackablePacket&>(rhs), rhs._date);
	}

	friend bool operator!=(const C2S_GetShiftsByDay& lhs, const C2S_GetShiftsByDay& rhs) {
		return !(lhs == rhs);
	}
};
