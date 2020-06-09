#pragma once
#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
#include "Date.h"
#include <stdexcept>

using namespace Serialization;
using namespace Binary;

/**
 * Serializable class extending date with additional (hour, minute, second) fields
 * 
 */
class DateTime : public Date {
protected:
	uint8_t _hour{};
	uint8_t _minute{};
	uint8_t _second{};

public:
	Type getType() const override {
		return Type::_DateTime;
	}
	/**
	 * Construct a new DateTime object with default date (00:00 1st January 1970)
	 * 
	 */
	DateTime() : DateTime(1970, 1, 1, 0, 0, 0) { }
	/**
	 * Construct a new DateTime object from the given Date
	 * 
	 * @param date date to be used
	 */
	DateTime(const Date& date) : DateTime(date, 0, 0, 0) {}
	/**
	 * Construct a new DateTime object from the given date and time
	 * 
	 * @param date Date object
	 * @param hour hour
	 * @param minute minute
	 * @param second second
	 */
	DateTime(const Date& date, int hour, int minute, int second) : DateTime(date.getYear(), date.getMonth(), date.getDay(), hour, minute, second) {}
	/**
	 * Construct a new Date Time object
	 * 
	 * @param year year
	 * @param month month
	 * @param day day
	 * @param hour hour
	 * @param minute minute
	 * @param second second
	 */
	DateTime(int year, int month, int day, int hour, int minute, int second) : Date(year, month, day) {
		setHour(hour);
		setMinute(minute);
		setSecond(second);
	}
	/**
	 * Gets the object's hour 
	 * 
	 * @return uint8_t 
	 */
	uint8_t getHour() const {
		return _hour;
	}
	/**
	 * Sets the hour
	 * 
	 * @param hour 
	 */
	void setHour(int hour) {
		if (hour < 0 || hour > 23) {
			throw std::invalid_argument("Invalid hour");
		}
		_hour = hour;
	}
	/**
	 * Gets the object's minute
	 * 
	 * @return uint8_t 
	 */
	uint8_t getMinute() const {
		return _minute;
	}
	/**
	 * Sets the minute
	 * 
	 * @param minute 
	 */
	void setMinute(int minute) {
		if (minute < 0 || minute > 59) {
			throw std::invalid_argument("Invalid minute");
		}
		_minute = minute;
	}
	/**
	 * Gets the object's second 
	 * 
	 * @return uint8_t 
	 */
	uint8_t getSecond() const {
		return _second;
	}
	/**
	 * Sets the second
	 * 
	 * @param second 
	 */
	void setSecond(int second) {
		if (second < 0 || second > 59) {
			throw std::invalid_argument("Invalid second");
		}
		_second = second;
	}
	/**
	 * Copies this DateTime as a Date
	 * 
	 * @return Date 
	 */
	Date getDate() const {
		return Date(*this);
	}


	std::ostream& serialize(std::ostream& destination) const override {
		Date::serialize(destination);
		write_primitive(destination, _hour);
		write_primitive(destination, _minute);
		write_primitive(destination, _second);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		Date::deserialize(source);
		setHour(read_primitive<uint8_t>(source));
		setMinute(read_primitive<uint8_t>(source));
		setSecond(read_primitive<uint8_t>(source));
		return source;
	}


	friend bool operator==(const DateTime& lhs, const DateTime& rhs)
	{
		return static_cast<const Date&>(lhs) == static_cast<const Date&>(rhs)
			&& lhs._hour == rhs._hour
			&& lhs._minute == rhs._minute
			&& lhs._second == rhs._second;
	}

	friend bool operator!=(const DateTime& lhs, const DateTime& rhs)
	{
		return !(lhs == rhs);
	}

	friend bool operator<(const DateTime& lhs, const DateTime& rhs) {
		return std::tie(static_cast<const Date&>(lhs), lhs._hour, lhs._minute, lhs._second) < std::tie(
			static_cast<const Date&>(rhs), rhs._hour, rhs._minute, rhs._second);
	}

	friend bool operator<=(const DateTime& lhs, const DateTime& rhs) {
		return !(rhs < lhs);
	}

	friend bool operator>(const DateTime& lhs, const DateTime& rhs) {
		return rhs < lhs;
	}

	friend bool operator>=(const DateTime& lhs, const DateTime& rhs) {
		return !(lhs < rhs);
	}

	DateTime(const DateTime& other)
		: Date{other},
		  _hour{other._hour},
		  _minute{other._minute},
		  _second{other._second} {}

	DateTime(DateTime&& other) noexcept
		: Date{std::move(other)},
		  _hour{other._hour},
		  _minute{other._minute},
		  _second{other._second} {}

	DateTime& operator=(const DateTime& other) {
		if (this == &other)
			return *this;
		Date::operator =(other);
		_hour = other._hour;
		_minute = other._minute;
		_second = other._second;
		return *this;
	}

	DateTime& operator=(DateTime&& other) noexcept {
		if (this == &other)
			return *this;
		Date::operator =(std::move(other));
		_hour = other._hour;
		_minute = other._minute;
		_second = other._second;
		return *this;
	}
};
