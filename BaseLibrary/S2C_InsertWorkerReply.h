#pragma once
#pragma once
#include <utility>


#include "Serializable.h"
#include "binary.h"
#include "TransactionReply.h"


using namespace Serialization;
using namespace Binary;
/**
 * Server-to-client response to insert a ShiftWorker object
 * 
 */
class S2C_InsertWorkerReply : public TransactionReply {
protected:
	ShiftWorker _worker;
public:
	Type getType() const override {
		return Type::_S2C_InsertWorkerReply;
	}

	/**
	 * Gets the queried ShiftWorker by reference
	 * 
	 * @return ShiftWorker& 
	 */
	virtual ShiftWorker& getWorker()
	{
		return _worker;
	}

	/**
	 * Gets a copy of the queried ShiftWorker
	 * 
	 * @return ShiftWorker 
	 */
	virtual ShiftWorker getWorker() const
	{
		return _worker;
	}
	/**
	 * Sets the queried ShiftWorker
	 * 
	 * @param worker 
	 */
	virtual void setWorker(const ShiftWorker& worker)
	{
		_worker = worker;
	}

	S2C_InsertWorkerReply() : TransactionReply(0){}
	/**
	 * Construct a new response to insert a ShiftWorker
	 * 
	 * @param requestId request id
	 * @param worker queried ShiftWorker
	 */
	S2C_InsertWorkerReply(int requestId, ShiftWorker worker)
		: TransactionReply(requestId), _worker(std::move(worker)) {}

	std::ostream& serialize(std::ostream& destination) const override
	{
		TransactionReply::serialize(destination);
		_worker.serialize(destination);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override
	{
		TransactionReply::deserialize(source);
	
		_worker = get_instance<ShiftWorker>(source);
		
		return source;
	}


	friend bool operator==(const S2C_InsertWorkerReply& lhs, const S2C_InsertWorkerReply& rhs) {
		return std::tie(static_cast<const TransactionReply&>(lhs), lhs._worker) == std::tie(
			static_cast<const TransactionReply&>(rhs), rhs._worker);
	}

	friend bool operator!=(const S2C_InsertWorkerReply& lhs, const S2C_InsertWorkerReply& rhs) {
		return !(lhs == rhs);
	}
};
