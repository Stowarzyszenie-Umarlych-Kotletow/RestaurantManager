/**
 * Provide in-memory alternatives to iostream streams/buffers
 */

#pragma once
#include <iostream>
#include "types.h"


/**
 * In-memory byte buffer that handles both input and output operations
 */
class memory_buf : public std::basic_streambuf<char> {
private:
	size_t _capacity;
	size_t _length;
	pbyte_t _data;

protected:
	/**
	 * Updates internal buffer pointers to the currently allocated buffer
	 */
	void updateBuffer();

	/**
	 * Calculate buffer offset given a relative seek offset
	 */
	char* calcOffset(char* base, char* now, char* end, std::streamoff off, std::ios_base::seekdir way);

	/* override base functions */

	std::streampos
	seekpos(std::streampos sp, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;

	std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way,
	                       std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;


public:
	memory_buf() {
		_capacity = _length = 0;
		_data = nullptr;
	}
	/**
	 * Construct a memory buffer by taking ownership of the byte buffer
	 * @param buffer byte buffer
	 * @param length buffer length
	 */
	memory_buf(const pbyte_t buffer, size_t length) {
		_capacity = length;
		_data = buffer;
		setLength(length, false);
	}

	~memory_buf() {
		setData(nullptr, 0);
	}
	/**
	 * Disposes the allocated buffer
	 */
	void dispose() {
		setData(nullptr, 0);
	}
	/**
	 * Gets the current length of the buffer
	 */
	size_t getLength() const {
		return _length;
	}
	/**
	 * Gets the length of the allocated internal buffer
	 */
	size_t getCapacity() const {
		return _capacity;
	}
	/**
	 * Sets length of the buffer, expanding if necessary
	 * @param length required length
	 * @param expand whether to allocate a larger buffer if necessary
	 */
	bool setLength(size_t length, bool expand);

	/**
	 * Takes ownership of the specified buffer
	 * @param data byte buffer
	 * @param length buffer length
	 */
	void setData(pbyte_t data, size_t length);

	/**
	 * Allocates a new buffer of the specified length
	 */
	void setData(size_t length) {
		setData(new byte_t[length], length);
	}
	/**
	 * Gets the current 0-indexed position of input operations
	 */
	std::streampos getInPosition() const {
		return gptr() - eback();
	}
	/**
	 * Gets the current 0-indexed position of output operations
	 */
	std::streampos getOutPosition() const {
		return pptr() - pbase();
	}
	/**
	 * Return a pointer to the internal byte buffer
	 */
	pbyte_t data() const {
		return _data;
	}
};

/**
 * Provides an in-memory input/output stream with std::iostream interface,
 * using memory_buf as the internal buffer.
 */
class memory_stream : public std::iostream {
public:
	memory_stream() : std::iostream(&_buffer) {
		rdbuf(&_buffer);
	}
	/*
	 * Initializes internal memory_buf by taking ownership of the provided byte buffer
	 * @param buffer byte buffer
	 * @param length buffer length
	 */
	memory_stream(const pbyte_t buffer, size_t length) :
		std::iostream(&_buffer),
		_buffer(buffer, length) {
		rdbuf(&_buffer);
	}
	/**
	 * Disposes the internal buffer
	 */
	void dispose() {
		_buffer.dispose();
	}
	/*
	 * Returns a reference to the internal buffer
	 */
	memory_buf& buffer() {
		return _buffer;
	}

private:
	memory_buf _buffer;
};
