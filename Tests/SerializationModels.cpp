#include "pch.h"


#include <codecvt>
#include <locale>

#include "CppUnitTest.h"
#include "../BaseLibrary/buffers.h"
#include "../BaseLibrary/Date.h"
#include "../BaseLibrary/DateTime.h"
#include "../BaseLibrary/net_constants.h"
#include "../BaseLibrary/serialization.h"
#include "../BaseLibrary/models.h"

namespace Serialization {
	class Serializable;
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(SerializationModels)
	{
	protected:
		memory_stream stream;
		
		template <typename T>
		T rand_bytes(int count) {
			T t;
			auto ptr = reinterpret_cast<char*>(&t);
			for(int i = 0; i < sizeof(T); i++) {
				*ptr++ = static_cast<char>(rand());
			}
			return t;
		}
		template <typename T>
		T rand_primitive(T min, T max) {
			Assert::IsTrue((max - min) != 0);
			return static_cast<T>(rand() % (max - min) + min);
		}
		Date rand_date() {
			return Date(rand_primitive(50, 3200), rand_primitive(1, 12), rand_primitive(1, 28));
		}

		DateTime rand_datetime() {
			return DateTime(rand_date(), rand_primitive(0, 23), rand_primitive(0, 59), rand_primitive(0, 59));
		}

		std::string rand_string() {
			size_t length = rand_primitive(0, 128);
			char* buf = new char[length];
			for (size_t i = 0; i < length; i++)
				buf[i] = rand_primitive<byte_t>(20u, 200u);

			std::string str(buf);
			delete[] buf;
			return str;
		}
		std::wstring s2ws(const std::string& str)
		{
			return std::wstring(str.begin(), str.end());
		}

		std::wstring rand_wstring(int id = -1) {
			if (id == -1)
				id = rand() % 3;
			switch(rand() % 3) {
			case 0:
				return L"";
			case 1:
				return L"zażółćgęśląjążń";
			default:
				return s2ws(rand_string());
			}
		}

		identity_t rand_id() {
			return rand_primitive(0ULL, 124912494219ULL);
		}
		
		Shift rand_shift() {
			auto date = rand_datetime();
			date.setHour(22);
			return Shift(date, rand_primitive(1, 24 - date.getHour()), rand_wstring(), rand_id(), rand_id());
		}

		ShiftWorker rand_worker() {
			return ShiftWorker(rand_wstring(), rand_wstring(), rand_wstring(), rand_id());
		}
		
		template <typename T>
		void checkSerialization(const T& serializable) {
			stream.buffer().pubseekpos(0);
			serializable.serialize(stream);
			T other = Serialization::get_instance<T>(stream);
			std::wstring msg = L"Deserialized object does not equal to the original.";
			Assert::IsTrue(serializable == other, msg.c_str());
			Assert::IsFalse(serializable != other, msg.c_str());
		}
		
	public:
		SerializationModels() {
			srand(195120);
			stream.buffer().setLength(MAX_PACKET_SIZE, true);
			if(TypeInfo::TYPES.empty())
				register_models();
		}
		TEST_METHOD(SerializeDate)
		{
			checkSerialization(Date(2020, 06, 01));
			checkSerialization(Date(3001, 01, 31));
			checkSerialization(Date());
			for (int i = 0; i < 16; i++)
				checkSerialization(rand_date());
		}
		TEST_METHOD(SerializeDateTime) {
			checkSerialization(DateTime(2020, 01, 31, 01, 02, 03));
			checkSerialization(DateTime(3020, 12, 31, 23, 59, 59));
			checkSerialization(DateTime());
			for (int i = 0; i < 16; i++)
				checkSerialization(rand_datetime());
		}

		TEST_METHOD(SerializeShift) {
			checkSerialization(Shift(DateTime(2019, 02, 25, 13, 21, 39), 1, L"ASCII", 5001, 123));
			checkSerialization(Shift(DateTime(3050, 05, 21, 01, 03, 02), 5, L"zażółć gęślą jaźń", 19213, 12390));
			checkSerialization(Shift());
			for (int i = 0; i < 16; i++)
				checkSerialization(rand_shift());
		}

		TEST_METHOD(SerializeWorker) {
			checkSerialization(ShiftWorker(L"Jan", L"Kowalski", L"mgr", 591510));
			checkSerialization(ShiftWorker(L"Przemysław", L"Rozwałka", L"zażółczaćgęśląjaźń", 15159));
			checkSerialization(ShiftWorker());
			for (int i = 0; i < 16; i++)
				checkSerialization(rand_worker());
		}

		TEST_METHOD(SerializeAuthorize) {
			checkSerialization(C2S_Authorize("token"));
			checkSerialization(C2S_Authorize(rand_string()));
			checkSerialization(C2S_Authorize(""));
			checkSerialization(S2C_AuthorizeReply(125012512, true, UserPermissions::SUPER_USER));
			checkSerialization(S2C_AuthorizeReply(251951291, false, UserPermissions::None, "Error message!"));
		}

		TEST_METHOD(SerializeDeleteShift) {
			checkSerialization(C2S_DeleteShift(2512515));
			checkSerialization(C2S_DeleteShift(1));
			S2C_DeleteShiftReply reply(125215, 616);
			checkSerialization(reply);
			reply.setErrorMsg("Something went wrong");
			checkSerialization(reply);
			reply.setId(-1);
			checkSerialization(reply);
		}

		TEST_METHOD(SerializeDeleteWorker) {
			checkSerialization(C2S_DeleteWorker(1));
			checkSerialization(C2S_DeleteWorker(1259215));
			S2C_DeleteWorkerReply reply(21955129, 12);
			checkSerialization(reply);
			reply.setId(5120);
			checkSerialization(reply);
			reply.setErrorMsg("errrorrr!!!!!11");
			checkSerialization(reply);
		}

		TEST_METHOD(SerializeGetShiftsByDay) {
			checkSerialization(C2S_GetShiftsByDay(Date()));
			checkSerialization(C2S_GetShiftsByDay(DateTime()));
			checkSerialization(C2S_GetShiftsByDay(Date(5125, 12, 01)));
			S2C_GetShiftsReply reply(21125, Date());
			checkSerialization(reply);
			reply.setShifts({Shift(DateTime(), 5, L"Some job", 1, 5), Shift(DateTime(1,2,3,4,5,6), 7, L"Other job", 8, 9)});
			checkSerialization(reply);
			reply.setErrorMsg("?");
			checkSerialization(reply);
			reply.setRequestId(41241);
			checkSerialization(reply);
		}

		TEST_METHOD(SerializeGetWorkers) {
			checkSerialization(C2S_GetWorkers());
			S2C_GetWorkersReply reply(412421);
			checkSerialization(reply);
			reply.setWorkers({ {5, ShiftWorker()}, {124214, ShiftWorker(L"ążłą", L"żłźżó", L"łźżćó", 5)} });
			checkSerialization(reply);
			reply.setWorkers({});
			checkSerialization(reply);
			reply.setErrorMsg("Error!");
			checkSerialization(reply);
		}
		TEST_METHOD(SerializeInsertShift) {
			checkSerialization(C2S_InsertShift());
			checkSerialization(C2S_InsertShift(rand_shift(), true));
			S2C_InsertShiftReply reply(515215, rand_shift());
			checkSerialization(reply);
			reply.setErrorMsg("Something");
			checkSerialization(reply);
			reply.setShift(Shift(DateTime(), 12, L"ŁŻó", 16, 32));
			checkSerialization(reply);
		}

		TEST_METHOD(SerializeInsertWorker) {
			checkSerialization(C2S_InsertWorker());
			checkSerialization(C2S_InsertWorker(rand_worker(), true));
			S2C_InsertWorkerReply reply(515215, rand_worker());
			checkSerialization(reply);
			reply.setErrorMsg("Something");
			checkSerialization(reply);
			reply.setWorker(rand_worker());
			checkSerialization(reply);
		}

		TEST_METHOD(SerializeSyncClient) {
			S2C_ClientSync sync;
			checkSerialization(sync);
			size_t cnt = rand_primitive(1, 16);
			while (cnt--)
				sync.getRemovedShifts().insert(rand_id());
			checkSerialization(sync);
			cnt = rand_primitive(1, 16);
			while (cnt--)
				sync.getRemovedWorkers().insert(rand_id());
			checkSerialization(sync);
			cnt = rand_primitive(1, 16);
			while (cnt--)
				sync.getChangedShifts().insert(rand_shift());
			checkSerialization(sync);
			cnt = rand_primitive(1, 16);
			while (cnt--)
				sync.getChangedWorkers().insert(rand_worker());
			checkSerialization(sync);
			
		}

		TEST_METHOD(SerializePing) {
			checkSerialization(Ping());
			checkSerialization(PingReply());
		}
	};
}
