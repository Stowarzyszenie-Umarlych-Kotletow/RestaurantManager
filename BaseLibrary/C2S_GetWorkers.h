#pragma once
#include "Serializable.h"
#include "binary.h"
#include "TrackablePacket.h"
using namespace Serialization;
using namespace Binary;
/**
 * Client-to-server request to list all ShiftWorker objects
 * 
 */
class C2S_GetWorkers : public TrackablePacket {
protected:
public:
	Type getType() const override {
		return Type::_C2S_GetWorkers;
	}

	C2S_GetWorkers() = default;


	friend bool operator==(const C2S_GetWorkers& lhs, const C2S_GetWorkers& rhs) {
		return static_cast<const TrackablePacket&>(lhs) == static_cast<const TrackablePacket&>(rhs);
	}

	friend bool operator!=(const C2S_GetWorkers& lhs, const C2S_GetWorkers& rhs) {
		return !(lhs == rhs);
	}
};
