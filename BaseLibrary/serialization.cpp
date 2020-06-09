#include "serialization.h"

#include <map>


namespace Serialization {
	std::map<Type, TypeInfo> TypeInfo::TYPES;
	std::map<std::type_index, Type> TypeInfo::NATIVE_TYPES;
}
