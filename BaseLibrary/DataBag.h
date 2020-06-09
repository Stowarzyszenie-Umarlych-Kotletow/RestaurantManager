#pragma once
#include <string>
#include <map>
#include <memory>

/**
 * Class that stores data associatively in string-keyed dictionary
 * 
 */
class DataBag {
private:
	std::map<std::string, std::shared_ptr<void>> _map;
public:
	/**
	 * Clears all stored data
	 * 
	 */
	void clear();
	/**
	 * Remove the object stored in the given key
	 * 
	 * @param key key to be deleted
	 */
	void remove(std::string key);
	/**
	 * Stores a copy of the given object
	 * 
	 * @tparam T type to be stored
	 * @param key dictionary key
	 * @param value object to be stored
	 */
	template <typename T>
	void put(std::string key, const T& value) {
		_map[key] = std::shared_ptr<void>(new T(value));
	}
	/**
	 * Retrieves a stored object, using a fallback if not found
	 * 
	 * @tparam T type to be retrieved
	 * @param key dictionary key
	 * @param fallback fallback value
	 * @return T& reference to the retrieved object, or fallback if not found
	 */
	template <typename T>
	T& get(std::string key, T& fallback) const {
		auto it = _map.find(key);
		if (it != _map.end())
			return *std::static_pointer_cast<T>(it->second);
		return fallback;
	}
	/**
	 * Retrieves a stored object, using a fallback if not found
	 * 
	 * @tparam T type to be retrieved
	 * @param key dictionary key
	 * @param fallback fallback value
	 * @return T& const reference to the retrieved object, or fallback if not found
	 */
	template <typename T>
	const T& get(std::string key, const T& fallback) const {
		auto it = _map.find(key);
		if (it != _map.end())
			return *std::static_pointer_cast<T>(it->second);
		return fallback;
	}


};
