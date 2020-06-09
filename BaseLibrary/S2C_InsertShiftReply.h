#pragma once
#include "Serializable.h"
#include "binary.h"
#include "TransactionReply.h"


using namespace Serialization;
using namespace Binary;
/**
 * Server-to-client response to insert a Shift object
 * 
 */
class S2C_InsertShiftReply : public TransactionReply {
protected:
	Shift _shift;
public:
	Type getType() const override {
		return Type::_S2C_InsertShiftReply;
	}

	/**
	 * Gets the queried Shift object by reference
	 * 
	 * @return Shift& 
	 */
	virtual Shift& getShift()
	{
		return _shift;
	}
	/**
	 * Gets a copy of the queried Shift object
	 * 
	 * @return Shift 
	 */
	virtual Shift getShift() const
	{
		return _shift;
	}
	/**
	 * Sets the queried Shift object
	 * 
	 * @param shift 
	 */
	virtual void setShift(const Shift& shift)
	{
		_shift = shift;
	}

	S2C_InsertShiftReply() = default;
	/**
	 * Construct a new response to insert a Shift object
	 * 
	 * @param requestId request id
	 * @param shift queried object
	 */
	S2C_InsertShiftReply(int requestId, const Shift& shift)
		: TransactionReply(requestId), _shift(shift) {}

	std::ostream& serialize(std::ostream& destination) const override
	{
		TransactionReply::serialize(destination);
		_shift.serialize(destination);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override
	{
		TransactionReply::deserialize(source);
		_shift = get_instance<Shift>(source);
		return source;
	}


	friend bool operator==(const S2C_InsertShiftReply& lhs, const S2C_InsertShiftReply& rhs) {
		return std::tie(static_cast<const TransactionReply&>(lhs), lhs._shift) == std::tie(
			static_cast<const TransactionReply&>(rhs), rhs._shift);
	}

	friend bool operator!=(const S2C_InsertShiftReply& lhs, const S2C_InsertShiftReply& rhs) {
		return !(lhs == rhs);
	}
};
