#pragma once
#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
#include <stdexcept>

using namespace Serialization;
using namespace Binary;

/**
 * Serializable class representing a date (year, month, day)
 * 
 */
class Date : public Serializable {
protected:
	uint32_t _year;
	uint8_t _month;
	uint8_t _day;


public:
	Type getType() const override {
		return Type::_Date;
	}
	/**
	 * Construct a new Date object with default date (1st January 1970)
	 * 
	 */
	Date() : Date(1970, 1, 1) { }
	/**
	 * Construct a new Date object
	 * 
	 * @param year year
	 * @param month month
	 * @param day day
	 */
	Date(int year, int month, int day) {
		_year = year;
		_month = month;
		_day = day;
	}
	/**
	 * Gets the object's year 
	 * 
	 * @return uint32_t 
	 */
	uint32_t getYear() const {
		return _year;
	}
	/**
	 * Sets the year
	 * 
	 * @param year 
	 */
	void setYear(int year) {
		if (year < 0) {
			throw std::invalid_argument("Invalid year");
		}
		_year = year;
	}
	/**
	 * Gets the object's month 
	 * 
	 * @return uint8_t 
	 */
	uint8_t getMonth() const {
		return _month;
	}
	/**
	 * Sets the month
	 * 
	 * @param month 
	 */
	void setMonth(int month) {
		if (month < 1 || month > 12) {
			throw std::invalid_argument("Invalid month");
		}
		_month = month;
	}
	/**
	 * Gets the object's day 
	 * 
	 * @return uint8_t 
	 */
	uint8_t getDay() const {
		return _day;
	}
	/**
	 * Sets the day
	 * 
	 * @param day 
	 */
	void setDay(int day) {
		if (day < 1 || day > 31) {
			throw std::invalid_argument("Invalid day");
		}
		_day = day;
	}

	std::ostream& serialize(std::ostream& destination) const override {
		Serializable::serialize(destination);
		write_primitive(destination, _year);
		write_primitive(destination, _month);
		write_primitive(destination, _day);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		Serializable::deserialize(source);
		setYear(read_primitive<uint32_t>(source));
		setMonth(read_primitive<uint8_t>(source));
		setDay(read_primitive<uint8_t>(source));
		return source;
	}


	friend bool operator==(const Date& lhs, const Date& rhs)
	{
		return lhs._year == rhs._year
			&& lhs._month == rhs._month
			&& lhs._day == rhs._day;
	}

	friend bool operator!=(const Date& lhs, const Date& rhs)
	{
		return !(lhs == rhs);
	}

	friend bool operator<(const Date& lhs, const Date& rhs) {
		return std::tie(lhs._year, lhs._month, lhs._day) < std::tie(rhs._year, rhs._month, rhs._day);
	}

	friend bool operator<=(const Date& lhs, const Date& rhs) {
		return !(rhs < lhs);
	}

	friend bool operator>(const Date& lhs, const Date& rhs) {
		return rhs < lhs;
	}

	friend bool operator>=(const Date& lhs, const Date& rhs) {
		return !(lhs < rhs);
	}

	Date(const Date& other)
		: _year{other._year},
		  _month{other._month},
		  _day{other._day} {}

	Date(Date&& other) noexcept
		: _year{other._year},
		  _month{other._month},
		  _day{other._day} {}

	Date& operator=(const Date& other) {
		if (this == &other)
			return *this;
		_year = other._year;
		_month = other._month;
		_day = other._day;
		return *this;
	}

	Date& operator=(Date&& other) noexcept {
		if (this == &other)
			return *this;
		_year = other._year;
		_month = other._month;
		_day = other._day;
		return *this;
	}
};
