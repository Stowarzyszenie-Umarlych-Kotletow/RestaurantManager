#pragma once
#include <string>
#include <stdexcept>
#include "binary.h"

/**
 * Namespace that contains all serialization logic
 */
namespace Serialization {
	using namespace Binary;
	/**
	 * Enumeration of all serializable type with their ids
	 * 
	 */
	enum class Type : uint8_t {
		Unknown = 0,
		_Ping = 1,
		_PingReply = 2,
		_TrackablePacket,
		_TransactionReply,

		_RestaurantManager,
		_Date,
		_DateTime,
		_Shift,
		_ShiftWorker,
		_C2S_Authorize,
		_S2C_AuthorizeReply,
		_C2S_GetShiftsByDay,
		_S2C_GetShiftsReply,
		_C2S_InsertShift,
		_S2C_InsertShiftReply,
		_C2S_DeleteShift,
		_S2C_DeleteShiftReply,
		_C2S_GetWorkers,
		_S2C_GetWorkersReply,
		_C2S_InsertWorker,
		_S2C_InsertWorkerReply,
		_C2S_DeleteWorker,
		_S2C_DeleteWorkerReply,
		_S2C_ClientSync,
	};
	/**
	 * Base-class for all serializable objects
	 * 
	 */
	class Serializable {

	public:
		virtual ~Serializable() = default;
		/**
	     * @return Type of this serializable object
	     */
		virtual Type getType() const = 0;


		/**
		 * Serializes this object into a binary output stream
		 * @param destination binary output stream
		 * @return output stream
		 */
		virtual std::ostream& serialize(std::ostream& destination) const {
			write_primitive(destination, this->getType());
			return destination;
		}

		/**
		 * Deserializes this object from a binary input stream
		 * @param source binary input stream
		 * @return input stream
		 */
		virtual std::istream& deserialize(std::istream& source) {
			auto type = static_cast<uint8_t>(this->getType());
			char targetType;
			read_primitive(source, &targetType);
			if (targetType != type)
				throw std::runtime_error(
					"incompatible object type #" + std::to_string(type) + " <- #" + std::to_string(targetType));
			return source;
		}


		friend bool operator==(const Serializable& lhs, const Serializable& rhs) {
			return true;
		}

		friend bool operator!=(const Serializable& lhs, const Serializable& rhs) {
			return !(lhs == rhs);
		}
	};

	std::ostream& operator<<(std::ostream& dst, const Serializable& obj);

	std::istream& operator>>(std::istream& src, Serializable& obj);


}
