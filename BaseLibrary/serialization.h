#pragma once
#include <map>
#include <functional>
#include <memory>
#include <typeindex>

#include "Serializable.h"

namespace Serialization {

	/**
	 * Struct to hold the object graph of serializable types
	 */
	struct TypeInfo {
	public:
		static std::map<std::type_index, Type> NATIVE_TYPES;
		static std::map<Type, TypeInfo> TYPES;
		bool registered = false;
		/**
		 * Function that constructs a serializable type and returns its shared pointer
		 */
		std::function<std::shared_ptr<Serializable>()> constructor = nullptr;
		/**
		 * Parent-type of this type
		 */
		Type parent = Type::Unknown;
		/**
		 * Compiler-generated name for this type
		 */
		const char* name = nullptr;
	};

	/**
	 * Builds a function that constructs a Serializable object of given type
	 * @tparam T type that implements Serializable
	 * @return callable function that returns a shared pointer to newly constructed instance
	 */
	template <typename T>
	static std::function<std::shared_ptr<Serializable>()> build_constructor() {
		return []() {
			return std::dynamic_pointer_cast<Serializable>(std::make_shared<T>());
		};
	}

	/**
	 * Checks whether one type is assignable from the other
	 * @param lhs left-hand-side
	 * @param rhs right-hand-side
	 * @return true if rhs can be assigned to lhs, false otherwise
	 */
	static bool is_assignable(Type lhs, Type rhs) {
		if (lhs == rhs) return true;
		std::map<Type, TypeInfo>::iterator it;
		while ((it = TypeInfo::TYPES.find(rhs)) != TypeInfo::TYPES.end()) {
			rhs = it->second.parent;
			if (lhs == rhs) return true;
		}
		return false;
	}

	/**
	 * Registers a given type as Serializable
	 * @tparam T Serializable type
	 * @param type the enum tag of this type
	 * @param parent optionally - the parent (base class) of this type
	 */
	template <typename T>
	static void register_type(Type type, Type parent = Type::Unknown) {
		const auto& it = TypeInfo::TYPES.find(type);
		if (it != TypeInfo::TYPES.end() && it->second.registered) {
			throw std::runtime_error("the specified type is already registered");
		}
		TypeInfo::TYPES[type] = {true, build_constructor<T>(), parent, typeid(T).name()};
	}

	/**
	 * Resolves and registers information about serializable type `T`
	 * @tparam T Serializable object
	 */
	template <typename T>
	static Type get_type() {
		auto type = std::type_index(typeid(T));
		auto it = TypeInfo::NATIVE_TYPES.find(type);
		if (it == TypeInfo::NATIVE_TYPES.end()) {
			return (TypeInfo::NATIVE_TYPES[type] = T().getType());
		}
		return it->second;
	}

	/**
	 * Returns a display name for the given serializable type.
	 * @param type Serializable type
	 */
	static std::string get_type_name(Type type) {
		const auto& it = TypeInfo::TYPES.find(type);
		if (it != TypeInfo::TYPES.end() && it->second.name)
			return it->second.name;
		return "#" + std::to_string(static_cast<int>(type));
	}

	template <typename T>
	static void register_type() {
		register_type<T>(get_type<T>());
	}

	template <typename T, typename TParent>
	static void register_derived() {
		register_type<T>(get_type<T>(), get_type<TParent>());
	}

	template <typename T>
	static void register_derived(Type parent) {
		register_type<T>(get_type<T>(), parent);
	}

	/**
	* Registers a given base type as Serializable
	* @tparam T Serializable type
	* @param type the enum tag of this type
	*/
	template <typename T>
	static void register_base_type(Type type, Type parent = Type::Unknown) {
		const auto& it = TypeInfo::TYPES.find(type);
		if (it != TypeInfo::TYPES.end() && it->second.registered)
			throw std::runtime_error("the specified type is already registered");
		TypeInfo::TYPES[type] = {true, nullptr, parent, typeid(T).name()};
		TypeInfo::NATIVE_TYPES[std::type_index(typeid(T))] = type;
	}


	/**
	 * Creates a new instance of given Serializable type casted as base type T
	 * @tparam T the base-class pointer type
	 * @param baseType base-class serializable type
	 * @param type target type
	 * @return shared pointer to an instance of given type
	 */
	template <typename T>
	static std::shared_ptr<T> new_instance(Type baseType, Type type) {
		auto& TYPES = TypeInfo::TYPES;
		if (baseType != Type::Unknown && TYPES.find(baseType) == TYPES.end())
			throw std::runtime_error("unregistered base type " + get_type_name(baseType));
		if (TYPES.find(type) == TYPES.end())
			throw std::runtime_error("unregistered target type " + get_type_name(type));
		if (!is_assignable(baseType, type))
			throw std::runtime_error(
				"target type is not derived from given base type " + get_type_name(baseType) + " <- " +
				get_type_name(type));
		auto ctor = TYPES[type].constructor;
		if (ctor == nullptr)
			throw std::runtime_error("target type is a base type " + get_type_name(type));
		auto ptr = ctor();
		return std::dynamic_pointer_cast<T>(ptr);
	}

	/**
	 * Non-generic function to create a new instance of given Serializable type
	 * @param type Serializable type
	 */
	static std::shared_ptr<Serializable> new_instance(Type type) {
		auto& TYPES = TypeInfo::TYPES;
		if (TYPES.find(type) == TYPES.end())
			throw std::runtime_error("unregistered target type " + get_type_name(type));
		auto ctor = TYPES[type].constructor;
		if (ctor == nullptr)
			throw std::runtime_error("target type is a base type " + get_type_name(type));
		auto ptr = ctor();
		return ptr;

	}

	template <typename T>
	static std::shared_ptr<T> new_instance(Type type) {
		return new_instance<T>(type, type);
	}

	/**
	* Creates a new instance of given Serializable type from input stream identifier
	* @tparam T the base-class pointer type
	* @param baseType base type
	* @param source input stream
	* @return shared pointer to an instance of given type
	*/
	template <typename T>
	static std::shared_ptr<T> new_instance(Type baseType, std::istream& source) {
		Type type;
		read_primitive(source, &type);
		source.putback(static_cast<char>(type));
		return new_instance<T>(baseType, type);
	}

	/**
	 * Creates a new instance of given Serializable type from input stream identifier
	 * @tparam T the Serializable class
	 * @param source input stream
	 * @return shared pointer to an instance of given type
	 */
	template <typename T>
	static std::shared_ptr<T> new_instance(std::istream& source) {
		return new_instance<T>(get_type<T>(), source);
	}


	/**
	 * Creates a new instance of given Serializable type from input stream identifier
	 * @param source input stream
	 * @return shared pointer to an instance of given type
	 */
	static std::shared_ptr<Serializable> new_instance(std::istream& source) {
		Type type;
		read_primitive(source, &type);
		source.putback(static_cast<char>(type));
		return new_instance(type);
	}

	/**
	 * Deserializes a new instance of given Serializable type from input stream identifier
	 * @param src input stream
	 * @return shared pointer to an instance of given type
	 */
	template <typename T>
	static T get_instance(std::istream& src) {
		auto instance = new_instance<T>(src);
		instance->deserialize(src);
		return *instance;
	}
	/**
	 * Creates a function to read the target type `T` (or a derived one) from the source stream.
	 */
	template <typename T>
	static std::function<T(std::istream&)> get_type_deserializer() {
		return get_instance<T>;
	}
}
