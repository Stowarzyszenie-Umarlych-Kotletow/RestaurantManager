#pragma once
#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
using namespace Serialization;
using namespace Binary;
/**
 * Response to a Ping request
 * 
 */
class PingReply : public Serializable {
protected:
public:
	Type getType() const override {
		return Type::_PingReply;
	}

	PingReply() {}

};
