#pragma once
#include <utility>


#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
#include "TrackablePacket.h"
using namespace Serialization;
using namespace Binary;
/**
 * Base-class for trackable transaction replies with success flags and error message.
 * 
 */
class TransactionReply : public TrackablePacket {
protected:
	bool _success;
	std::string _errorMsg;
public:
	TransactionReply() : TransactionReply(0) {}
	/**
	 * Construct a new successful TransactionReply object
	 * 
	 * @param requestId request id
	 */
	TransactionReply(int requestId) : TransactionReply(requestId, true) {}
	/**
	 * Construct a new failure TransactionReply object and sets the error message
	 * 
	 * @param requestId request id
	 * @param msg error message
	 */
	TransactionReply(int requestId, std::string msg) : TransactionReply(requestId, false, std::move(msg)) {}
	/**
	 * Construct a new Transaction Reply object
	 * 
	 * @param requestId request id
	 * @param success whether the transaction succeeded
	 * @param msg error message
	 */
	TransactionReply(int requestId, bool success, std::string msg = "") : TrackablePacket(requestId) {
		_success = success;
		_errorMsg = std::move(msg);
	}

	/**
	 * 
	 * @return Whether the transaction succeeded
	 */
	virtual bool isSuccess() const {
		return _success;
	}
	/**
	 * Sets the success flag
	 * 
	 * @param success Whether the transaction succeeded
	 */
	virtual void setSuccess(const bool success) {
		_success = success;
	}
	/**
	 * Gets the error message
	 * 
	 * @return std::string error message
	 */
	virtual std::string getErrorMsg() const {
		return _errorMsg;
	}
	/**
	 * Sets the error message and modifies the success flag to false
	 * 
	 * @param msg 
	 */
	virtual void setErrorMsg(const std::string& msg) {
		setSuccess(false);
		_errorMsg = msg;
	}

	std::ostream& serialize(std::ostream& destination) const override {
		TrackablePacket::serialize(destination);
		write_primitive(destination, _success);
		write_string(destination, _errorMsg);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		TrackablePacket::deserialize(source);
		read_primitive(source, &_success);
		_errorMsg = read_string(source);
		return source;
	}

	friend bool operator==(const TransactionReply& lhs, const TransactionReply& rhs) {
		return std::tie(static_cast<const TrackablePacket&>(lhs), lhs._success, lhs._errorMsg) == std::tie(
			static_cast<const TrackablePacket&>(rhs), rhs._success, rhs._errorMsg);
	}

	friend bool operator!=(const TransactionReply& lhs, const TransactionReply& rhs) {
		return !(lhs == rhs);
	}
};
