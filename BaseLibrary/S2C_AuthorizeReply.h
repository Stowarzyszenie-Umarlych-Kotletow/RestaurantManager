#pragma once
#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
#include "TrackablePacket.h"
#include "TransactionReply.h"
#include "UserPermissions.h"

using namespace Serialization;
using namespace Binary;
/**
 * Server-to-client reply to a token-based authorization request
 * 
 */
class S2C_AuthorizeReply : public TransactionReply {
protected:
	UserPermissions _permissions;
public:
	Type getType() const override {
		return Type::_S2C_AuthorizeReply;
	}

	S2C_AuthorizeReply() : S2C_AuthorizeReply(0) {}
	/**
	 * Construct a new authorize response
	 * 
	 * @param requestId request id
	 */
	S2C_AuthorizeReply(int requestId) : S2C_AuthorizeReply(requestId, true, UserPermissions::None) {}
	/**
	 * Construct a new authorize response
	 * 
	 * @param requestId request id
	 * @param success whether authorization succeeded
	 * @param permissions permissions granted
	 * @param msg error message
	 */
	S2C_AuthorizeReply(int requestId, bool success, UserPermissions permissions, const std::string& msg = "")
		: TransactionReply(requestId, success, msg) {
		_permissions = permissions;
	}
	/**
	 * Gets the permissions granted
	 * 
	 * @return UserPermissions permissions
	 */
	UserPermissions getPermissions() const {
		return _permissions;
	};
	/**
	 * Sets the permissions granted
	 * 
	 * @param permissions permissions
	 */
	void setPermissions(UserPermissions permissions) {
		_permissions = permissions;
	}

	std::ostream& serialize(std::ostream& destination) const override {
		TransactionReply::serialize(destination);
		write_primitive(destination, _permissions);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		TransactionReply::deserialize(source);
		read_primitive(source, &_permissions);
		return source;
	}


	friend bool operator==(const S2C_AuthorizeReply& lhs, const S2C_AuthorizeReply& rhs) {
		return std::tie(static_cast<const TransactionReply&>(lhs), lhs._permissions) == std::tie(
			static_cast<const TransactionReply&>(rhs), rhs._permissions);
	}

	friend bool operator!=(const S2C_AuthorizeReply& lhs, const S2C_AuthorizeReply& rhs) {
		return !(lhs == rhs);
	}
};
