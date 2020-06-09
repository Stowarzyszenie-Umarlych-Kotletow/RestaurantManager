#pragma once
#include <functional>
#include <iostream>
#include <set>

namespace Serialization {
	namespace Binary {
		/**
		 * Writes a primitive to the target stream
		 * 
		 * @tparam T primitive type to be written
		 * @param destination destination stream
		 * @param value value to be written
		 * @return size_t number of bytes written
		 */
		template <typename T>
		static size_t write_primitive(std::ostream& destination, T value) {
			if (!destination.good())
				throw std::runtime_error("invalid destination stream");
			destination.write(reinterpret_cast<char*>(&value), sizeof(T));
			return sizeof(T);
		}
		/**
		 * Reads a primitive from the target stream
		 * 
		 * @tparam T primitive type to be read
		 * @param source source stream
		 * @param obj pointer to store the read value
		 * @return size_t number of bytes read
		 */
		template <typename T>
		static size_t read_primitive(std::istream& source, T* obj) {
			if (!source.good())
				throw std::runtime_error("invalid destination stream");
			source.read(reinterpret_cast<char*>(obj), sizeof(T));
			return sizeof(T);
		}
		/**
		 * Reads a primitive from the target stream
		 * 
		 * @tparam T primitive type to be read
		 * @param source source stream
		 * @return T read value
		 */
		template <typename T>
		static T read_primitive(std::istream& source) {
			T obj;
			if (!source.good())
				throw std::runtime_error("invalid destination stream");
			source.read(reinterpret_cast<char*>(&obj), sizeof(T));
			return obj;
		}
		/**
		 * Reads a length-prefixed string from the source stream
		 * 
		 * @param source source stream
		 * @return std::string read string
		 */
		static std::string read_string(std::istream& source) {
			size_t len;
			read_primitive(source, &len);
			char* buf = new char[len];
			source.read(buf, len);
			auto str = std::string(buf, len);
			delete[] buf;
			return str;
		}
		/**
		 * Writes a length-prefixed string to the destination stream
		 * 
		 * @param destination destination stream
		 * @param str string to be written
		 * @return size_t number of bytes written
		 */
		static size_t write_string(std::ostream& destination, const std::string& str) {
			size_t len = str.size();
			write_primitive(destination, len);
			if (len > 0)
				destination.write((char*)str.c_str(), len);
			return len + sizeof(size_t);
		}

		/**
		 * Writes a length-prefixed wide string to the destination stream
		 * 
		 * @param destination destination stream
		 * @param str string to be written
		 * @return size_t number of bytes written
		 */
		static size_t write_wstring(std::ostream& destination, const std::wstring& str) {
			size_t len = str.size();
			write_primitive(destination, len);
			if (len > 0)
				destination.write((char*)str.c_str(), len*sizeof(wchar_t));
			return len;
		}

		/**
		 * Reads a length-prefixed wide string from the source stream
		 * 
		 * @param source source stream
		 * @return std::wstring read string
		 */
		static std::wstring read_wstring(std::istream& source) {
			size_t len;
			read_primitive(source, &len);
			wchar_t* buf = new wchar_t[len];
			source.read((char*)buf, len*sizeof(wchar_t));
			auto str = std::wstring(buf, len);
			delete[] buf;
			return str;
		}
		/**
		 * Writes a {@code std::set<T>} of primitives into the destination stream
		 * 
		 * @tparam T primitive type
		 * @param destination destination stream
		 * @param set collection
		 */
		template <typename T>
		static void write_set_primitive(std::ostream& destination, const std::set<T>& set) {
			size_t len = set.size();
			write_primitive(destination, len);
			for(auto obj : set) {
				write_primitive(destination, obj);
			}
		}
		/**
		 * Writes a std::set<T> of Serializable objects into the destination stream
		 * 
		 * @tparam T serializable type
		 * @param destination destination stream
		 * @param set collection
		 */
		template <typename T>
		static void write_set_typed(std::ostream& destination, const std::set<T>& set) {
			size_t len = set.size();
			write_primitive(destination, len);
			for (const auto& obj : set) {
				obj.serialize(destination);
			}
		}
		/**
		 * Reads a std::set<T> from the source stream by applying the callback function
		 * 
		 * @tparam T element type
		 * @param source source stream
		 * @param set output collection
		 * @param callback deserializer for T
		 */
		template <typename T>
		static void read_set(std::istream& source, std::set<T>& set, std::function<T(std::istream&)> callback) {
			set.clear();
			auto len = read_primitive<size_t>(source);
			for(size_t i = 0; i < len; i++) {
				set.insert(callback(source));
			}
		}
		/**
		 * Reads a std::set<T> of primitives from the source stream
		 * 
		 * @tparam T primitive element type
		 * @param source source stream
		 * @param set output collection
		 */
		template <typename T>
		static void read_set_primitive(std::istream& source, std::set<T>& set) {
			read_set<T>(source, set, [](std::istream& src) {
				return read_primitive<T>(src);
			});
		}


	}
};
