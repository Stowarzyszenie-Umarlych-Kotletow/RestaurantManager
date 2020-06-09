#include <iostream>
#include "Serializable.h"

namespace Serialization {
	std::ostream& operator<<(std::ostream& dst, const Serializable& obj) {
		return obj.serialize(dst);
	}

	std::istream& operator>>(std::istream& src, Serializable& obj) {
		return obj.deserialize(src);
	}
};
