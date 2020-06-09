#include <codecvt>
#include <wx/datetime.h>
#include <wx/msgdlg.h>

#include "Date.h"
#include "DateTime.h"
#include "TransactionReply.h"

Date asDateModel(const wxDateTime& date) {
	return Date(date.GetYear(), date.GetMonth() + 1, date.GetDay());
}

wxDateTime asWxDateTimeModel(const Date& date) {
	wxDateTime dateTime;
	dateTime.Set(date.getDay(), wxDateTime::Month(date.getMonth() - 1), date.getYear());
	return dateTime;
}

std::string asHourString(const DateTime& date) {
	return std::to_string(date.getHour()) + ":00";
}

// trim from beginning of string (left)
std::wstring& ltrim(std::wstring& s, const wchar_t* t) {
	s.erase(0, s.find_first_not_of(t));
	return s;
}

std::wstring& rtrim(std::wstring& s, const wchar_t* t) {
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

// trim from both ends of string (right then left)
std::wstring& trim(std::wstring& s, const wchar_t* t) {
	return ltrim(rtrim(s, t), t);
}


void showMessageBox(std::string title, std::string text, wxWindow* parent = nullptr, long style = 5L) {
	wxMessageBox(text, title, style, parent);
}

void showTransactionMessageBox(std::string title, std::string text, const TransactionReply& reply, wxWindow* parent) {
	auto style = 5L | (reply.isSuccess() ? wxICON_INFORMATION : wxICON_EXCLAMATION);
	if (!reply.isSuccess())
		text += "\n" + reply.getErrorMsg();
	showMessageBox(title, text, parent, style);
}

void showExceptionMessageBox(std::string title, std::string text, const std::exception& exception, wxWindow* parent,
                             bool fatal) {
	auto style = 5L | wxICON_ERROR;
	text += "\n" + std::string(exception.what());
	showMessageBox(title, text, parent, style);
	if (fatal && parent)
		parent->Close();
}

std::wstring s2ws(const std::string& str) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}
