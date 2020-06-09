#pragma once
#pragma once
#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
using namespace Serialization;
using namespace Binary;
/**
 * Base-class for tracking request-response packets by id
 * 
 */
class TrackablePacket : public Serializable {
protected:
	int _requestId;
public:
	/**
	 * Construct a new Trackable Packet
	 * 
	 * @param requestId request id
	 */
	TrackablePacket(int requestId) {
		_requestId = requestId;
	}

	TrackablePacket() : TrackablePacket(0) {}
	/**
	 * Sets the Request Id
	 * 
	 * @param requestId 
	 */
	void setRequestId(int requestId) {
		_requestId = requestId;
	}
	/**
	 * Gets the object's Request Id 
	 * 
	 * @return int Request Id 
	 */
	int getRequestId() const {
		return _requestId;
	}

	std::ostream& serialize(std::ostream& destination) const override {
		Serializable::serialize(destination);
		write_primitive(destination, _requestId);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		Serializable::deserialize(source);
		read_primitive(source, &_requestId);
		return source;
	}

	friend bool operator==(const TrackablePacket& lhs, const TrackablePacket& rhs) {
		return lhs._requestId == rhs._requestId;
	}

	friend bool operator!=(const TrackablePacket& lhs, const TrackablePacket& rhs) {
		return !(lhs == rhs);
	}

};
