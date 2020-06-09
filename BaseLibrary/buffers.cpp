#include "buffers.h"

void memory_buf::updateBuffer() {
	auto* data = reinterpret_cast<char*>(_data);
	setg(data, data, data + _length);
	setp(data, data, data + _length);
}

char* memory_buf::calcOffset(char* base, char* now, char* end, std::streamoff off, std::ios_base::seekdir way) {
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

std::streampos memory_buf::seekpos(std::streampos sp, std::ios_base::openmode which) {
	return seekoff(sp, std::ios_base::beg, which);
}

std::streampos memory_buf::seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which) {
	std::streampos absolute = off;

	std::streampos lastPos;

	if (which & std::ios_base::in) {
		auto* base = eback();
		auto* end = egptr();
		auto* now = calcOffset(base, gptr(), end, off, way);
		setg(base, now, end);
		lastPos = now - base;
	}
	if (which & std::ios_base::out) {
		auto* base = pbase();
		auto* end = epptr();
		auto* now = calcOffset(base, pptr(), end, off, way);
		setp(base, now, end);
		lastPos = now - base;
	}
	return lastPos;
}

bool memory_buf::setLength(size_t length, bool expand) {
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

void memory_buf::setData(pbyte_t data, size_t length) {
	if (_data && _data != data)
		delete[] _data;
	_length = _capacity = length;
	_data = data;

	updateBuffer();
}
