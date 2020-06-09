#pragma once
#include "Serializable.h"
#include "binary.h"
#include "TransactionReply.h"

using namespace Serialization;
using namespace Binary;
/**
 * Server-to-client response to deleting a Shift object
 * 
 */
class S2C_DeleteShiftReply : public TransactionReply {
protected:
	identity_t _id = 0;
public:
	Type getType() const override {
		return Type::_S2C_DeleteShiftReply;
	}

	/**
	 * Gets the Shift id
	 * 
	 * @return identity_t 
	 */
	virtual identity_t getId() const {
		return _id;
	}
	/**
	 * Sets the Shift id
	 * 
	 * @param id 
	 */
	virtual void setId(identity_t id) {
		_id = id;
	}

	S2C_DeleteShiftReply() = default;
	/**
	 * Construct a new response to deleting a Shift object
	 * 
	 * @param requestId request id
	 * @param shiftId shift id
	 */
	S2C_DeleteShiftReply(int requestId, identity_t shiftId)
		: TransactionReply(requestId), _id(shiftId) {}


	std::ostream& serialize(std::ostream& destination) const override {
		TransactionReply::serialize(destination);
		write_primitive(destination, _id);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		TransactionReply::deserialize(source);
		read_primitive(source, &_id);
		return source;
	}


	friend bool operator==(const S2C_DeleteShiftReply& lhs, const S2C_DeleteShiftReply& rhs) {
		return std::tie(static_cast<const TransactionReply&>(lhs), lhs._id) == std::tie(
			static_cast<const TransactionReply&>(rhs), rhs._id);
	}

	friend bool operator!=(const S2C_DeleteShiftReply& lhs, const S2C_DeleteShiftReply& rhs) {
		return !(lhs == rhs);
	}
};
