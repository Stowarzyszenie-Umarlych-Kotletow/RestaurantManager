#pragma once
#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
#include "types.h"
#include <stdexcept>
#include <utility>

using namespace Serialization;
using namespace Binary;
/**
 * Class representing a worker associated with a Shift
 * 
 */
class ShiftWorker : public Serializable {
protected:
	identity_t _id;
	std::wstring _firstName;
	std::wstring _lastName;
	std::wstring _title;


public:
	/**
	 * Gets the id of this worker
	 * 
	 * @return identity_t 
	 */
	virtual identity_t getId() const {
		return _id;
	}
	/**
	 * Sets the id of this worker
	 * 
	 * @param workerId 
	 */
	virtual void setId(const identity_t workerId) {
		_id = workerId;
	}
	/**
	 * Gets the first name
	 * 
	 * @return std::wstring 
	 */
	virtual std::wstring getFirstName() const {
		return _firstName;
	}
	/**
	 * Sets the first name
	 * 
	 * @param firstName 
	 */
	virtual void setFirstName(const std::wstring& firstName) {
		_firstName = firstName;
	}
	/**
	 * Gets the last name
	 * 
	 * @return std::wstring 
	 */
	virtual std::wstring getLastName() const {
		return _lastName;
	}
	/**
	 * Sets the last name
	 * 
	 * @param lastName 
	 */
	virtual void setLastName(const std::wstring& lastName) {
		_lastName = lastName;
	}
	/**
	 * Gets the title of this worker
	 * 
	 * @return std::wstring 
	 */
	virtual std::wstring getTitle() const {
		return _title;
	}
	/**
	 * Sets the title of this worker
	 * 
	 * @param title 
	 */
	virtual void setTitle(const std::wstring& title) {
		_title = title;
	}

	Type getType() const override {
		return Type::_ShiftWorker;
	}
	/**
	 * Returns a string representation of this object
	 * 
	 * @return std::wstring 
	 */
	virtual std::wstring toString() const
	{
		return L"[" + std::to_wstring(_id) + L"] " + getFirstName() + L" " + getLastName() + L" (" + getTitle() + L")";
	}

	ShiftWorker() : ShiftWorker(L"", L"", L"") {}

	ShiftWorker(std::wstring firstName, std::wstring lastName,
	            std::wstring title, identity_t id = 0)
		: _id(id),
		  _firstName(std::move(firstName)),
		  _lastName(std::move(lastName)),
		  _title(std::move(title)) { }

	std::ostream& serialize(std::ostream& destination) const override {
		Serializable::serialize(destination);
		write_primitive(destination, _id);
		write_wstring(destination, _firstName);
		write_wstring(destination, _lastName);
		write_wstring(destination, _title);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		Serializable::deserialize(source);
		read_primitive(source, &_id);
		_firstName = read_wstring(source);
		_lastName = read_wstring(source);
		_title = read_wstring(source);
		return source;
	}


	friend bool operator==(const ShiftWorker& lhs, const ShiftWorker& rhs) {
		return lhs._id == rhs._id
			&& lhs._firstName == rhs._firstName
			&& lhs._lastName == rhs._lastName
			&& lhs._title == rhs._title;
	}

	friend bool operator!=(const ShiftWorker& lhs, const ShiftWorker& rhs) {
		return !(lhs == rhs);
	}


	friend bool operator<(const ShiftWorker& lhs, const ShiftWorker& rhs) {
		return std::tie(lhs._firstName, lhs._lastName, lhs._title, lhs._id) <
			std::tie(rhs._firstName, rhs._lastName, rhs._title, rhs._id);
	}

	friend bool operator<=(const ShiftWorker& lhs, const ShiftWorker& rhs) {
		return !(rhs < lhs);
	}

	friend bool operator>(const ShiftWorker& lhs, const ShiftWorker& rhs) {
		return rhs < lhs;
	}

	friend bool operator>=(const ShiftWorker& lhs, const ShiftWorker& rhs) {
		return !(lhs < rhs);
	}

	ShiftWorker(const ShiftWorker& other)
		: _id{other._id},
		  _firstName{other._firstName},
		  _lastName{other._lastName},
		  _title{other._title} {}

	ShiftWorker(ShiftWorker&& other) noexcept
		: _id{other._id},
		  _firstName{std::move(other._firstName)},
		  _lastName{std::move(other._lastName)},
		  _title{std::move(other._title)} {}

	ShiftWorker& operator=(const ShiftWorker& other) {
		if (this == &other)
			return *this;
		_id = other._id;
		_firstName = other._firstName;
		_lastName = other._lastName;
		_title = other._title;
		return *this;
	}

	ShiftWorker& operator=(ShiftWorker&& other) noexcept {
		if (this == &other)
			return *this;
		_id = other._id;
		_firstName = std::move(other._firstName);
		_lastName = std::move(other._lastName);
		_title = std::move(other._title);
		return *this;
	}
};
