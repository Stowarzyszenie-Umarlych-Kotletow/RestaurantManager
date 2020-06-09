#pragma once
#include <set>
#include <utility>


#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
#include "Shift.h"
#include "TransactionReply.h"


using namespace Serialization;
using namespace Binary;
/**
 * Server-to-client response that lists all Shift objects in the given day
 * 
 */
class S2C_GetShiftsReply : public TransactionReply {
protected:
	std::set<Shift> _shifts;
	Date _date;
public:
	Type getType() const override {
		return Type::_S2C_GetShiftsReply;
	}

	S2C_GetShiftsReply() : S2C_GetShiftsReply(0, Date()) {}
	/**
	 * Construct a new response that lists all Shifts in the given day
	 * 
	 * @param requestId request id
	 * @param date queried day
	 */
	S2C_GetShiftsReply(int requestId, Date date) : TransactionReply(requestId), _date(date) {}
	/**
	 * Gets the Shifts in the queried day
	 * 
	 * @return std::set<Shift> 
	 */
	std::set<Shift> getShifts() const {
		return _shifts;
	}
	/**
	 * Gets the Shifts in the queried day by reference
	 * 
	 * @return std::set<Shift>& 
	 */
	std::set<Shift>& getShifts() {
		return _shifts;
	}
	/**
	 * Sets the Shifts
	 * 
	 * @param shifts 
	 */
	void setShifts(const std::set<Shift>& shifts) { _shifts = shifts; }
	/**
	 * Gets the queried date
	 * 
	 * @return Date 
	 */
	Date getDate() const
	{
		return _date;
	}
	/**
	 * Gets the queried date by reference
	 * 
	 * @return Date& 
	 */
	Date& getDate()
	{
		return _date;
	}

	std::ostream& serialize(std::ostream& destination) const override {
		TransactionReply::serialize(destination);
		_date.serialize(destination);
		write_set_typed(destination, _shifts);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		TransactionReply::deserialize(source);
		auto date = new_instance<Date>(source);
		date->deserialize(source);
		_date = *date;
		read_set(source, _shifts, get_type_deserializer<Shift>());
		return source;
	}


	friend bool operator==(const S2C_GetShiftsReply& lhs, const S2C_GetShiftsReply& rhs) {
		return std::tie(static_cast<const TransactionReply&>(lhs), lhs._shifts, lhs._date) == std::tie(
			static_cast<const TransactionReply&>(rhs), rhs._shifts, rhs._date);
	}

	friend bool operator!=(const S2C_GetShiftsReply& lhs, const S2C_GetShiftsReply& rhs) {
		return !(lhs == rhs);
	}
};
