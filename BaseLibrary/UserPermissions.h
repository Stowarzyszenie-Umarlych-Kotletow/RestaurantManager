#pragma once
#include <cstdint>
#include <string>

/**
 * Enumeration that specifies the user permissions for a client
 * 
 */
enum class UserPermissions : uint16_t {
	None = 0,
	View = 1,
	Insert = 2,
	Edit = 4,
	Delete = 8,
	NORMAL_USER = View,
	SUPER_USER = NORMAL_USER | Insert | Edit | Delete
};

UserPermissions operator|(UserPermissions lhs, UserPermissions rhs);

UserPermissions operator&(UserPermissions lhs, UserPermissions rhs);

/**
 * Returns the UserPermissions in a short string representation
 */
std::string getPermissionsString(UserPermissions permissions);
