#pragma once
#include <utility>
#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
#include "TrackablePacket.h"
using namespace Serialization;
using namespace Binary;
/**
 * Client-to-server packet that authorizes the client with a token
 * 
 */
class C2S_Authorize : public TrackablePacket {
protected:
	std::string _token;
public:
	Type getType() const override {
		return Type::_C2S_Authorize;
	}

	C2S_Authorize() = default;
	/**
	 * Construct a new authorize packet with a token
	 * 
	 * @param token auth token
	 */
	C2S_Authorize(std::string token) {
		_token = std::move(token);
	}
	/**
	 * Gets the object's auth token 
	 * 
	 * @return std::string 
	 */
	std::string getToken() const {
		return _token;
	};
	/**
	 * Sets the auth token
	 * 
	 * @param token 
	 */
	void setToken(std::string token) {
		_token = std::move(token);
	}

	std::ostream& serialize(std::ostream& destination) const override {
		TrackablePacket::serialize(destination);
		write_string(destination, _token);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		TrackablePacket::deserialize(source);
		_token = read_string(source);
		return source;
	}


	friend bool operator==(const C2S_Authorize& lhs, const C2S_Authorize& rhs) {
		return std::tie(static_cast<const TrackablePacket&>(lhs), lhs._token) == std::tie(
			static_cast<const TrackablePacket&>(rhs), rhs._token);
	}

	friend bool operator!=(const C2S_Authorize& lhs, const C2S_Authorize& rhs) {
		return !(lhs == rhs);
	}
};
