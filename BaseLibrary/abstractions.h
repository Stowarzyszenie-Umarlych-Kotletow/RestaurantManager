#pragma once
#include <iostream>
#include "types.h"

class memory_buf : public std::basic_streambuf<char> {
private:
	size_t _capacity;
	size_t _length;
	pbyte_t _data;

	void updateBuffer() {
		setg((char*)_data, (char*)_data, (char*)_data + _length);
		setp((char*)_data, (char*)_data, (char*)_data + _length);
	}

protected:
	std::streampos
	seekpos(std::streampos sp, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override {
		return seekoff(sp, std::ios_base::beg, which);
	}

	char* calcOffset(char* base, char* now, char* end, std::streamoff off, std::ios_base::seekdir way) {
		if (way == std::ios_base::beg) {
			now = base + off;
		}
		else if (way == std::ios_base::cur) {
			now += off;
		}
		else {
			now = end + off;
		}

		if (now < base || now > end)
			throw std::length_error("Invalid position");
		return now;
	}

	std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way,
	                       std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override {
		std::streampos absolute = off;

		std::streampos lastPos;

		if (which & std::ios_base::in) {
			auto base = eback();
			auto end = egptr();
			auto now = calcOffset(base, gptr(), end, off, way);
			setg(base, now, end);
			lastPos = now - base;
		}
		if (which & std::ios_base::out) {
			auto base = pbase();
			auto end = epptr();
			auto now = calcOffset(base, pptr(), end, off, way);
			setp(base, now, end);
			lastPos = now - base;
		}
		return lastPos;
	}


public:
	memory_buf() {
		_capacity = _length = 0;
		_data = nullptr;
	}

	memory_buf(const pbyte_t p, size_t l) {
		_capacity = l;
		_data = p;
		setLength(l, false);
	}

	~memory_buf() {
		if (_data)
			delete[] _data;
	}

	void dispose() {
		setData(nullptr, 0);
	}

	size_t getLength() {
		return _length;
	}

	size_t getCapacity() {
		return _capacity;
	}

	bool setLength(size_t length, bool expand) {
		if (!_data || length > _capacity) {
			if (!expand)
				return false;
			delete[] _data;
			_data = new byte_t[length];
			_length = _capacity = length;
		}
		else {
			_length = length;
		}
		updateBuffer();

		return true;
	}

	void setData(pbyte_t data, size_t length) {
		if (_data && _data != data)
			delete[] _data;
		_length = _capacity = length;
		_data = data;

		updateBuffer();
	}

	void setData(size_t length) {
		setData(new byte_t[length], length);
	}

	std::streampos getInPosition() const {
		return gptr() - eback();
	}

	std::streampos getOutPosition() const {
		return pptr() - pbase();
	}

	pbyte_t data() const {
		return _data;
	}
};

class memory_stream : public std::iostream {
public:
	memory_stream() : std::iostream(&_buffer) {
		rdbuf(&_buffer);
	}

	memory_stream(const pbyte_t p, size_t l) :
		std::iostream(&_buffer),
		_buffer(p, l) {
		rdbuf(&_buffer);
	}

	void dispose() {
		_buffer.dispose();
	}

	memory_buf& buffer() {
		return _buffer;
	}

private:
	memory_buf _buffer;
};
