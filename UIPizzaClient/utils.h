#pragma once
#include <codecvt>
#include <locale>
#include <wx/datetime.h>
#include "Date.h"

/**
 * Method that converts wxDateTime obj into Date obj
 * @param date date as wxDateTime obj
 * @returns Date obj
 */
Date asDateModel(const wxDateTime& date);
/**
 * Method that converts Date obj into wxDateTime obj
 * @param date date as Date obj
 * @returns wxDateTime obj
 */
wxDateTime asWxDateTimeModel(const Date& date);
/**
 * Method that converts hour from date into clean HH:00 format
 * @param date date as DateTime obj
 * @returns std::string
 */
std::string asHourString(const DateTime& date);
/**
 * Method that trims from end of string (right)
 * @param s input string as std::wstring
 * @param t whitespaces array
 * @returns std::wstring
 */
std::wstring& rtrim(std::wstring& s, const wchar_t* t = L" \t\n\r\f\v");
/**
 * Method that trims from beginning of string (left)
 * @param s input string as std::wstring
 * @param t whitespaces array
 * @returns std::wstring
 */
std::wstring& ltrim(std::wstring& s, const wchar_t* t = L" \t\n\r\f\v");
/**
 * Method that trims from both ends of string (right then left)
 * @param s input string as std::wstring
 * @param t whitespaces array
 * @returns std::wstring
 */
std::wstring& trim(std::wstring& s, const wchar_t* t = L" \t\n\r\f\v");
/**
 * Method for showing message boxes
 * @param title messageBox title
 * @param text messageBox text
 * @param parent onShown window pointer
 */
void showMessageBox(std::string title, std::string text, wxWindow* parent = nullptr);
/**
 * Method for showing server-pushed message boxes
 * @param title messageBox title
 * @param text messageBox text
 * @param reply server reply
 * @param parent onShown window pointer
 */
void showTransactionMessageBox(std::string title, std::string text, const TransactionReply& reply,
                               wxWindow* parent = nullptr);
/**
 * Method for showing exception message boxes
 * @param title messageBox title
 * @param text messageBox text
 * @param exception exception
 * @param parent onShown window pointer
 * @param fatal whether the exception is fatal
 */
void showExceptionMessageBox(std::string title, std::string text, const std::exception& exception,
                             wxWindow* parent = nullptr, bool fatal = false);
/**
 * @param str std::string
 * @returns std::wstring
 */
std::wstring s2ws(const std::string& str);
/**
 * @param wstr std::wstring
 * @returns std::string
 */
std::string ws2s(const std::wstring& wstr);
