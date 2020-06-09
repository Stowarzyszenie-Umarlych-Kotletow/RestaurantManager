#pragma once
#include "Serializable.h"
#include "serialization.h"
#include "binary.h"
#include "DateTime.h"
#include "types.h"
#include <stdexcept>
#include <utility>

using namespace Serialization;
using namespace Binary;

/**
 * Class that represents a Shift in the schedule
 * 
 */
class Shift : public Serializable {
protected:
	identity_t _id{};
	DateTime _startTime;
	uint8_t _workHours{};
	identity_t _workerId{};
	std::wstring _jobName;


public:
	Type getType() const override {
		return Type::_Shift;
	}

	Shift() : Shift(DateTime(), 1, L"") { }
	/**
	 * Construct a new Shift object
	 * 
	 * @param startTime starting time
	 * @param workHours duration
	 * @param jobName name of the job
	 * @param workerId id of the worker
	 * @param id id of the shift
	 */
	Shift(DateTime startTime, int workHours, std::wstring jobName, identity_t workerId = 0, identity_t id = 0) {
		_id = id;
		setStartTime(std::move(startTime));
		setWorkHours(workHours);
		setWorkerId(workerId);
		_jobName = std::move(jobName);
	}

	/* The accessors below shouldn't be virtual, since they need to be ran from the constructor */
	/**
	 * Gets the Shift's start time by reference
	 * 
	 * @return DateTime& 
	 */
	DateTime& getStartTime() {
		return _startTime;
	}
	/**
	 * Gets the Shift's start time by const reference
	 * 
	 * @return const DateTime& 
	 */
	const DateTime& getStartTime() const {
		return _startTime;
	}
	/**
	 * Gets the Shift's end time
	 * 
	 * @return DateTime 
	 */
	DateTime getEndTime() const {
		DateTime time = _startTime;
		time.setHour(time.getHour() + getWorkHours());
		return time;
	}
	/**
	 * Sets the start time
	 * 
	 * @param startTime 
	 */
	void setStartTime(DateTime startTime) {
		_startTime = std::move(startTime);
	}
	/**
	 * Gets the duration (in hours)
	 * 
	 * @return uint8_t hours
	 */
	uint8_t getWorkHours() const {
		return _workHours;
	}
	/**
	 * Sets the duration (in hours)
	 * 
	 * @param workHours 
	 */
	void setWorkHours(const uint8_t workHours) {
		if (workHours <= 0 || _startTime.getHour() + workHours > 23)
			throw std::invalid_argument("Invalid shift duration");
		_workHours = workHours;
	}
	/**
	 * Gets the associated worker id
	 * 
	 * @return identity_t 
	 */
	identity_t getWorkerId() const {
		return _workerId;
	}
	/**
	 * Sets the associated worker id
	 * 
	 * @param workerId 
	 */
	void setWorkerId(const identity_t workerId) {
		_workerId = workerId;
	}
	/**
	 * Gets the id
	 * 
	 * @return identity_t 
	 */
	identity_t getId() const {
		return _id;
	}
	/**
	 * Sets the id
	 * 
	 * @param id 
	 */
	void setId(const identity_t id) {
		_id = id;
	}

	/**
	 * Gets the name of this job
	 * 
	 * @return std::wstring 
	 */
	virtual std::wstring getJobName() const
	{
		return _jobName;
	}
	/**
	 * Sets the name of this job
	 * 
	 * @param jobName 
	 */
	virtual void setJobName(std::wstring jobName)
	{
		_jobName = jobName;
	}

	std::ostream& serialize(std::ostream& destination) const override {
		Serializable::serialize(destination);
		write_primitive(destination, _id);
		_startTime.serialize(destination);
		write_primitive(destination, _workHours);
		write_primitive(destination, _workerId);
		write_wstring(destination, _jobName);
		return destination;
	}

	std::istream& deserialize(std::istream& source) override {
		Serializable::deserialize(source);
		read_primitive(source, &_id);
		_startTime.deserialize(source);
		setWorkHours(read_primitive<uint8_t>(source));
		read_primitive(source, &_workerId);
		_jobName = read_wstring(source);
		return source;
	}


	friend bool operator==(const Shift& lhs, const Shift& rhs)
	{
		return lhs._id == rhs._id
			&& lhs.getStartTime() == rhs.getStartTime()
			&& lhs._workHours == rhs._workHours
			&& lhs._workerId == rhs._workerId
			&& lhs._jobName == rhs._jobName;
	}

	friend bool operator!=(const Shift& lhs, const Shift& rhs)
	{
		return !(lhs == rhs);
	}


	friend bool operator<(const Shift& lhs, const Shift& rhs)
	{
		return std::tie(lhs._startTime, lhs._workHours, lhs._jobName, lhs._workerId, lhs._id) < std::tie(
			rhs._startTime, rhs._workHours, rhs._jobName, rhs._workerId, rhs._id);
	}

	friend bool operator<=(const Shift& lhs, const Shift& rhs)
	{
		return !(rhs < lhs);
	}

	friend bool operator>(const Shift& lhs, const Shift& rhs)
	{
		return rhs < lhs;
	}

	friend bool operator>=(const Shift& lhs, const Shift& rhs)
	{
		return !(lhs < rhs);
	}
};
