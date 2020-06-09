#include "UserPermissions.h"

#include <cstdint>

UserPermissions operator|(UserPermissions lhs, UserPermissions rhs) {
	return static_cast<UserPermissions>(static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
}

UserPermissions operator&(UserPermissions lhs, UserPermissions rhs) {
	return static_cast<UserPermissions>(static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs));
}

std::string getPermissionsString(UserPermissions permissions) {
	const auto none = UserPermissions::None;
	if(permissions == none) {
		return "NONE";
	}
	std::string str;
	if((permissions & UserPermissions::Insert) != none) {
		str += "C";
	}
	if ((permissions & UserPermissions::View) != none) {
		str += "R";
	}
	if ((permissions & UserPermissions::Edit) != none) {
		str += "U";
	}
	if ((permissions & UserPermissions::Delete) != none) {
		str += "D";
	}
	return str;
}
