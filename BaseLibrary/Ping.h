#pragma once
#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
using namespace Serialization;
using namespace Binary;
/**
 * Basic Ping packet that forces the other endpoint to send a PingReply
 * 
 */
class Ping : public Serializable {
protected:
public:
	Type getType() const override {
		return Type::_Ping;
	}

	Ping() {}

};
