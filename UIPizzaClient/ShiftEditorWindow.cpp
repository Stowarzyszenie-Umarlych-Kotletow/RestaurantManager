#include <iostream>
#include "ShiftEditorWindow.h"
#include <regex>
#include "Shift.h"
#include "utils.h"
#include "WorkerEditorWindow.h"
#include "App.h"

// event table
wxBEGIN_EVENT_TABLE(ShiftEditorWindow, wxFrame)
	EVT_BUTTON(10001, onSubmitButtonClicked)
	EVT_CHOICE(10002, onChoice)
	EVT_BUTTON(10003, onDeleteButtonClicked)
	EVT_BUTTON(10004, onEditButtonClicked)
	EVT_CHOICE(10005, onStartHourChoice)
wxEND_EVENT_TABLE()

// creating form
///
/// 
///
ShiftEditorWindow::ShiftEditorWindow(App* app, identity_t shiftId) : wxFrame(
	nullptr, wxID_ANY, L"Rozjuszony Włoch - shift editor",
	wxPoint(30, 30), wxSize(350, 600)) {
	_app = app;
	_shiftId = shiftId;
	app->setShiftWindow(this);
	_panel = new wxPanel(this);

	wxArrayString st;
	for (int i = START_HOUR; i <= END_HOUR; i++) {
		wxString in = wxString(std::to_string(i) + ":00");
		st.Add(in);
	}
	auto date = _app->getMainWindow()->getCurrentDate();

	// setting background colour
	wxWindow::SetBackgroundColour(wxColour(0xff, 0x8a, 0x80));
	// fixed window size
	this->SetWindowStyle(wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));

	_textBoxTitle = new wxStaticText(_panel, wxID_ANY, "Shift editor", wxPoint(100, 30), wxSize(150, 40));
	// custom font


	_textBoxWorkerName = new wxStaticText(_panel, wxID_ANY, "Worker's name", wxPoint(45, 80), wxSize(150, 20));
	_buttonEdit = new wxButton(_panel, 10004, "Edit", wxPoint(10, 100), wxSize(40, 20));

	_choiceWorker = new wxChoice(_panel, 10002, wxPoint(65, 100), wxSize(200, 40));

	_textBoxWorkerJob = new wxStaticText(_panel, wxID_ANY, "Worker's job", wxPoint(45, 160), wxSize(150, 20));
	_textCtrlWorkerJob = new wxTextCtrl(_panel, wxID_ANY, "", wxPoint(65, 180), wxSize(200, 40));
	_textBoxHourStart = new wxStaticText(_panel, wxID_ANY, "Starting hour of a shift", wxPoint(45, 240),
	                                     wxSize(150, 20));
	_textBoxHourEnd = new wxStaticText(_panel, wxID_ANY, "Ending hour of a shift", wxPoint(45, 320), wxSize(150, 20));
	_textBoxDate = new wxStaticText(_panel, wxID_ANY, "Date of shift", wxPoint(45, 400), wxSize(150, 20));
	_choiceStartHour = new wxChoice(_panel, 10005, wxPoint(65, 260), wxSize(200, 40), st, 0, wxDefaultValidator,
	                                wxChoiceNameStr);
	_choiceEndHour = new wxChoice(_panel, wxID_ANY, wxPoint(65, 340), wxSize(200, 40), wxArrayString(), 0,
	                              wxDefaultValidator,
	                              wxChoiceNameStr);
	_calendar = new wxDatePickerCtrl(_panel, wxID_ANY, asWxDateTimeModel(date), wxPoint(65, 420), wxSize(120, 40));
	_buttonSubmit = new wxButton(_panel, 10001, "Submit", wxPoint(175, 490), wxSize(100, 30));
	_buttonDelete = new wxButton(_panel, 10003, "Delete shift", wxPoint(35, 490), wxSize(100, 30));


	wxFont font_title = wxFont(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	_textBoxTitle->SetFont(font_title);


	_buttonEdit->SetBackgroundColour(wxColour(0x80, 0xd8, 0xff));
	_buttonSubmit->SetBackgroundColour(wxColour(0, 255, 0));
	_buttonDelete->SetBackgroundColour((wxColour(255, 0, 0)));
	_buttonDelete->SetForegroundColour(wxColour(255, 255, 255));
	_buttonDelete->Hide();

	refreshInternal();
	updateEndHourChoice();
	if (shiftId != 0) {
		loadShift(shiftId);
	}
}

ShiftEditorWindow::~ShiftEditorWindow() {
	_app->setShiftWindow(nullptr);
}

void ShiftEditorWindow::loadShift(identity_t shiftId) {
	Shift shift;
	try {
		shift = _app->getClient().getShift(shiftId);
	}
	catch (std::exception&) {
		// shift doesn't exist
		return;
	}
	_buttonDelete->Show();
	auto wrkId = shift.getWorkerId();
	try {
		auto worker = _app->getClient().getWorker(wrkId);
		std::wstring workerData = worker.toString();

		auto foundShift = _choiceWorker->FindString(wxString(workerData));
		if (foundShift != wxNOT_FOUND)
			_choiceWorker->SetSelection(foundShift);
	}
	catch (std::exception&) {
		// worker doesn't exist
	}

	_textCtrlWorkerJob->SetValue(shift.getJobName());
	auto foundStart = _choiceStartHour->FindString(wxString(asHourString(shift.getStartTime())));
	if (foundStart != wxNOT_FOUND)
		_choiceStartHour->SetSelection(foundStart);

	auto foundEnd = _choiceEndHour->FindString(wxString(asHourString(shift.getEndTime())));
	if (foundEnd != wxNOT_FOUND)
		_choiceEndHour->SetSelection(foundEnd);

	auto date = shift.getStartTime().getDate();
	_calendar->SetValue(asWxDateTimeModel(date));
}

void ShiftEditorWindow::onSubmitButtonClicked(wxCommandEvent& evt) {
	auto name = _choiceWorker->GetString(_choiceWorker->GetCurrentSelection()).ToStdWstring();
	auto job = _textCtrlWorkerJob->GetValue().ToStdWstring();
	int start = std::atoi(_choiceStartHour->GetString(_choiceStartHour->GetCurrentSelection()).c_str());
	int end = std::atoi(_choiceEndHour->GetString(_choiceEndHour->GetCurrentSelection()).c_str());

	const auto& date = _calendar->GetValue();
	identity_t workerId = 0;
	try {
		workerId = getWorkerData(name).getId();
	}
	catch (std::exception&) {
		// invalid worker
	}

	if (rtrim(ltrim(job)) == L"" || start == 0 || end == 0) {
		wxMessageBox(wxT("Please fill all inputs!"), wxT("Correction needed"), wxICON_EXCLAMATION);
		evt.Skip();
		return;
	}
	int duration = start > end ? -1 : end - start;
	DateTime startTime(asDateModel(date), start, 0, 0);
	try {
		_app->getClient().queryInsertShift(Shift(startTime, duration, job, workerId, _shiftId), _shiftId != 0);
	}
	catch (std::exception& ex) {
		showExceptionMessageBox("Connection failure", "Error while inserting the shift", ex, this);
	}

	this->Close();
	evt.Skip();
}

void ShiftEditorWindow::openWorkerEditor(identity_t id) {
	if (!_app->getWorkerWindow()) {
		auto wew = new WorkerEditorWindow(_app, id);
		wew->Show();
		_app->setWorkerWindow(wew);
	}
	_app->getWorkerWindow()->SetFocus();
}


void ShiftEditorWindow::onChoice(wxCommandEvent& evt) {
	if (_choiceWorker->GetCurrentSelection() == (_choiceWorker->GetCount() - 1)) {
		openWorkerEditor();
	}
	evt.Skip();
}

void ShiftEditorWindow::onDeleteButtonClicked(wxCommandEvent& evt) {
	try {
		_app->getClient().queryDeleteShift(_shiftId);
	}
	catch (std::exception& ex) {
		showExceptionMessageBox("Connection failure", "Error while deleting the shift", ex, this);
	}
	this->Close();
	evt.Skip();
}

void ShiftEditorWindow::onEditButtonClicked(wxCommandEvent& evt) {
	auto name = _choiceWorker->GetString(_choiceWorker->GetCurrentSelection()).ToStdWstring();
	if (name == "") {
		evt.Skip();
		return;
	}
	auto worker = getWorkerData(name);
	openWorkerEditor(worker.getId());
	evt.Skip();
}

void ShiftEditorWindow::onStartHourChoice(wxCommandEvent& evt) {
	updateEndHourChoice();
}

void ShiftEditorWindow::updateEndHourChoice() {
	int start = START_HOUR + wxAtoi(std::to_string(_choiceStartHour->GetCurrentSelection()));
	wxArrayString en;
	for (int i = start + 1; i <= END_HOUR + 1; i++) {
		wxString in = wxString(std::to_string(i) + ":00");
		en.Add(in);
	}
	_choiceEndHour->Clear();
	_choiceEndHour->Append(en);
}

void ShiftEditorWindow::onShiftInserted(const S2C_InsertShiftReply& reply) {
	const std::string text = reply.isSuccess() ? "Shift updated successfully" : "Shift update failed";
	showTransactionMessageBox("Shift insert", text, reply, this);
}

void ShiftEditorWindow::onShiftDeleted(const S2C_DeleteShiftReply& reply) {
	const std::string text = reply.isSuccess() ? "Shift deleted successfully" : "shift deletion failed";
	showTransactionMessageBox("Shift delete", text, reply, this);
}

void ShiftEditorWindow::refresh() {
	wxMutexGuiEnter();
	refreshInternal();
	wxMutexGuiLeave();
}

ShiftWorker ShiftEditorWindow::getWorkerData(std::wstring name) {
	std::wregex reg(s2ws("\\[(.+)\\] (.+) (.*) \\((.*)\\)"));

	std::wsmatch match;
	if (std::regex_search(name, match, reg)) {
		std::wstring id = match[1].str();
		return ShiftWorker(match[2], match[3], match[4], std::stoi(id));
	}
	throw std::invalid_argument("Invalid name");
}

void ShiftEditorWindow::refreshInternal() {
	auto selection = _choiceWorker->GetSelection();
	_choiceWorker->Clear();
	wxArrayString items;
	auto listOfWorkers = _app->getClient().getWorkers();
	for (auto w : listOfWorkers) {
		wxString s = wxString(w.second.toString());
		items.Add(s);
	}
	items.Add(wxString("add new worker..."));

	_choiceWorker->Append(items);
	auto pos = static_cast<unsigned>(selection);
	if (pos < _choiceWorker->GetCount())
		_choiceWorker->SetSelection(pos);
}
