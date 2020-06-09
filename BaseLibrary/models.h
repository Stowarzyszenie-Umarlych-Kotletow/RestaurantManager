#pragma once
#include "C2S_Authorize.h"
#include "C2S_DeleteShift.h"
#include "C2S_DeleteWorker.h"
#include "C2S_GetShiftsByDay.h"
#include "C2S_GetWorkers.h"
#include "C2S_InsertShift.h"
#include "C2S_InsertWorker.h"
#include "Date.h"
#include "DateTime.h"
#include "serialization.h"

#include "Ping.h"
#include "PingReply.h"
#include "S2C_AuthorizeReply.h"
#include "S2C_DeleteShiftReply.h"
#include "S2C_DeleteWorkerReply.h"
#include "S2C_GetShiftsReply.h"
#include "S2C_GetWorkersReply.h"
#include "S2C_InsertShiftReply.h"
#include "S2C_InsertWorkerReply.h"
#include "S2C_ClientSync.h"
#include "Shift.h"
#include "ShiftWorker.h"
#include "TrackablePacket.h"
#include "TransactionReply.h"

namespace Serialization {
	/**
	 * Registers all Serializable models 
	 * 
	 */
	inline void register_models() {
		register_base_type<TrackablePacket>(Type::_TrackablePacket);
		register_base_type<TransactionReply>(Type::_TransactionReply, Type::_TrackablePacket);

		register_type<Ping>();
		register_type<PingReply>();
		register_type<Date>();
		register_derived<DateTime, Date>();
		register_type<Shift>();
		register_type<ShiftWorker>();
		register_type<S2C_ClientSync>();
		const auto trackable = Type::_TrackablePacket;
		register_derived<C2S_Authorize>(trackable);
		register_derived<C2S_DeleteShift>(trackable);
		register_derived<C2S_GetShiftsByDay>(trackable);
		register_derived<C2S_GetWorkers>(trackable);
		register_derived<C2S_InsertShift>(trackable);
		register_derived<C2S_InsertWorker>(trackable);
		register_derived<C2S_DeleteWorker>(trackable);
		const auto reply = Type::_TransactionReply;
		register_derived<S2C_AuthorizeReply>(reply);
		register_derived<S2C_DeleteShiftReply>(reply);
		register_derived<S2C_GetShiftsReply>(reply);
		register_derived<S2C_GetWorkersReply>(reply);
		register_derived<S2C_InsertShiftReply>(reply);
		register_derived<S2C_InsertWorkerReply>(reply);
		register_derived<S2C_DeleteWorkerReply>(reply);
		
	}
};
