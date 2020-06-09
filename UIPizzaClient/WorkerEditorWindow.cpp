#include <iostream>
#include "WorkerEditorWindow.h"
#include "Shift.h"
#include "utils.h"

// event table
wxBEGIN_EVENT_TABLE(WorkerEditorWindow, wxFrame)
	EVT_BUTTON(10001, onSubmitButtonClicked)
	EVT_BUTTON(10002, onDeleteButtonClicked)
wxEND_EVENT_TABLE()

// creating form
WorkerEditorWindow::WorkerEditorWindow(App* app, identity_t workerId) : wxFrame(
	nullptr, wxID_ANY, L"Rozjuszony Włoch - employee editor",
	wxPoint(30, 30), wxSize(350, 600)) {
	_app = app;
	app->setWorkerWindow(this);
	_workerId = workerId;

	_panel = new wxPanel(this);

	_textBoxTitle = new wxStaticText(_panel, wxID_ANY, "Employee data editor", wxPoint(70, 30), wxSize(150, 40));
	// custom font
	wxFont font_title = wxFont(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	_textBoxTitle->SetFont(font_title);

	// fixed window size
	this->SetWindowStyle(wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));

	_textBoxWorkerFirstName = new wxStaticText(_panel, wxID_ANY, "Worker's first name", wxPoint(45, 80),
	                                           wxSize(150, 20));
	_textBoxWorkerLastName = new wxStaticText(_panel, wxID_ANY, "Worker's last name", wxPoint(45, 160),
	                                          wxSize(150, 20));
	_textBoxWorkerTitle = new wxStaticText(_panel, wxID_ANY, "Worker's title", wxPoint(45, 240), wxSize(150, 20));

	_textCtrlWorkerFirstName = new wxTextCtrl(_panel, wxID_ANY, "", wxPoint(65, 100), wxSize(200, 40));
	_textCtrlWorkerLastName = new wxTextCtrl(_panel, wxID_ANY, "", wxPoint(65, 180), wxSize(200, 40));
	_textCtrlWorkerTitle = new wxTextCtrl(_panel, wxID_ANY, "", wxPoint(65, 260), wxSize(200, 40));

	_buttonSubmit = new wxButton(_panel, 10001, "Submit", wxPoint(155, 490), wxSize(100, 30));
	if (_workerId != 0) {
		_buttonDelete = new wxButton(_panel, 10002, "Delete worker", wxPoint(35, 490), wxSize(100, 30));
		_buttonDelete->SetBackgroundColour(wxColour(0xff, 0x52, 0x52));
	}
	wxWindow::SetBackgroundColour(wxColour(0x8c, 0x9e, 0xff));
	_buttonSubmit->SetBackgroundColour(wxColour(0x69, 0xf0, 0xae));

	try {
		auto worker = _app->getClient().getWorker(workerId);
		_textCtrlWorkerFirstName->SetValue(wxString(worker.getFirstName()));
		_textCtrlWorkerLastName->SetValue(wxString(worker.getLastName()));
		_textCtrlWorkerTitle->SetValue(wxString(worker.getTitle()));
	}
	catch (std::exception&) {
		_workerId = 0;
	}
}

WorkerEditorWindow::~WorkerEditorWindow() {
	_app->setWorkerWindow(nullptr);
}

void WorkerEditorWindow::onSubmitButtonClicked(wxCommandEvent& evt) {
	auto firstName = _textCtrlWorkerFirstName->GetValue().ToStdWstring();
	auto lastName = _textCtrlWorkerLastName->GetValue().ToStdWstring();
	auto title = _textCtrlWorkerTitle->GetValue().ToStdWstring();

	if (rtrim(ltrim(firstName)) == L"" || rtrim(ltrim(lastName)) == L"" || rtrim(ltrim(title)) == L"") {
		wxMessageBox(wxT("Please fill out all inputs!"), wxT("Correction needed"), wxICON_EXCLAMATION);
		evt.Skip();
		return;
	}
	try {
		_app->getClient().queryInsertWorker(ShiftWorker(trim(firstName), trim(lastName), trim(title), _workerId),
		                                    _workerId != 0);
	}
	catch (std::exception& ex) {
		showExceptionMessageBox("Connection failure", "Error while updating the worker", ex, this);
	}

	evt.Skip();
}

void WorkerEditorWindow::onDeleteButtonClicked(wxCommandEvent& evt) {
	try {
		_app->getClient().queryDeleteWorker(_workerId);
	}
	catch (std::exception& ex) {
		showExceptionMessageBox("Connection failure", "Error while deleting the worker", ex, this);
	}
	evt.Skip();
}

void WorkerEditorWindow::onWorkerInserted(const S2C_InsertWorkerReply& reply) {
	const std::string text = reply.isSuccess() ? "Worker updated successfully" : "Worker update failed";
	showTransactionMessageBox("Worker insert", text, reply, this);
	if (reply.isSuccess())
		this->Close();
}

void WorkerEditorWindow::onWorkerDeleted(const S2C_DeleteWorkerReply& reply) {
	const std::string text = reply.isSuccess() ? "Worker deleted successfully" : "Worker deletion failed";
	showTransactionMessageBox("Worker delete", text, reply, this);
	if (reply.isSuccess())
		this->Close();
}
