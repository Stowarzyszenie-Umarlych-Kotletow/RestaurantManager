#pragma once
#include "Serializable.h"
#include "binary.h"
#include "TransactionReply.h"

using namespace Serialization;
using namespace Binary;
/**
 * Server-to-client response to deleting a ShiftWorker object
 * 
 */
class S2C_DeleteWorkerReply : public TransactionReply {
protected:
	identity_t _id = 0;
public:
	Type getType() const override {
		return Type::_S2C_DeleteWorkerReply;
	}
	/**
	 * Gets the ShiftWorker id
	 * 
	 * @return identity_t 
	 */
	virtual identity_t getId() const
	{
		return _id;
	}
	/**
	 * Sets the ShiftWorker id
	 * 
	 * @param id 
	 */
	virtual void setId(identity_t id)
	{
		_id = id;
	}

	S2C_DeleteWorkerReply() = default;
	/**
	 * Construct a new response to ShiftWorker deletion
	 * 
	 * @param requestId request id
	 * @param id ShiftWorker id
	 */
	S2C_DeleteWorkerReply(int requestId, identity_t id)
		: TransactionReply(requestId), _id(id) {}


	
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


	friend bool operator==(const S2C_DeleteWorkerReply& lhs, const S2C_DeleteWorkerReply& rhs) {
		return std::tie(static_cast<const TransactionReply&>(lhs), lhs._id) == std::tie(
			static_cast<const TransactionReply&>(rhs), rhs._id);
	}

	friend bool operator!=(const S2C_DeleteWorkerReply& lhs, const S2C_DeleteWorkerReply& rhs) {
		return !(lhs == rhs);
	}
};
