#include <iostream>
#include <map>
#include "MainWindow.h"

#include <future>

#include "App.h"
#include "ShiftEditorWindow.h"
#include "ShiftCellRenderer.h"
#include "utils.h"

// events table
wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_BUTTON(101, onRefreshButtonClicked)
	EVT_BUTTON(104, onEditDataButtonClicked)
	EVT_DATE_CHANGED(21420, onDateChanged)
	EVT_SHOW(onShown)
	EVT_GRID_CMD_CELL_LEFT_CLICK(105, onCellLeftClicked)
	EVT_GRID_CMD_CELL_LEFT_DCLICK(105, onCellLeftDoubleClicked)
	EVT_BUTTON(106, onCurrDateButtonClicked)
	EVT_SIZE(onSizeChanged)
wxEND_EVENT_TABLE()


// form constructor
MainWindow::MainWindow(App* app) : wxFrame(nullptr, wxID_ANY, L"Rozjuszony Włoch - schedule",
                                           wxPoint(30, 30), wxSize(1000, 700)) {
	_app = app;
	wxWindow::SetBackgroundColour(wxColour(224, 247, 250));
	// layout settings
	_sizerHorizontal = new wxBoxSizer(wxHORIZONTAL);
	_sizerVertical = new wxBoxSizer(wxVERTICAL);
	// function buttons
	_btnRefresh = new wxButton(this, 101, "Refresh", wxPoint(30, 30), wxSize(120, 30));
	_btnEditData = new wxButton(this, 104, "Add new data", wxPoint(30, 30), wxSize(120, 30));
	_btnEditData->Hide();
	_btnCurrDate = new wxButton(this, 106, "Set today", wxPoint(30, 30), wxSize(120, 30));
	_textCurrentDate = new wxTextCtrl(this, wxID_ANY, "Loading", wxPoint(30, 30), wxSize(120, 50),
	                                  wxTE_READONLY | wxTE_CENTER | wxTE_MULTILINE | wxTE_NO_VSCROLL);

	auto firstNow = wxDateTime::Now(); // gets today's date
	_calendar = new wxDatePickerCtrl(this, 21420, firstNow, wxPoint(30, 30), wxSize(120, 40));

	// setting colours
	_btnRefresh->SetBackgroundColour(wxColour(0xff, 0xd5, 0x4f));
	_btnEditData->SetBackgroundColour(wxColour(0xcf, 0xd8, 0xdc));
	_textCurrentDate->SetBackgroundColour(wxColour(0x81, 0xd4, 0xfa));
	_btnCurrDate->SetBackgroundColour(wxColour(0xe1, 0xbe, 0xe7));

	_grid = new wxGrid(this, 105, wxPoint(10, 10), wxSize(700, 600));
	_grid->CreateGrid(0, 0);
	_grid->SetDefaultRenderer(new ShiftCellRenderer);
	_grid->SetDefaultCellBackgroundColour(this->GetBackgroundColour());
	_grid->EnableEditing(false);
	_grid->SetCellHighlightPenWidth(0);
	_grid->EnableGridLines(false);
	
	// putting main layout into the box
	SetSizer(_sizerHorizontal);
	_sizerHorizontal->AddSpacer(20);
	_sizerHorizontal->Add(_sizerVertical);
	_sizerVertical->AddSpacer(10);
	_sizerVertical->Add(_btnRefresh);
	_sizerVertical->AddSpacer(30);
	_sizerVertical->Add(_textCurrentDate);
	_sizerVertical->AddSpacer(30);
	_sizerVertical->Add(_btnEditData);
	_sizerVertical->AddSpacer(30);
	_sizerVertical->Add(_calendar);
	_sizerVertical->AddSpacer(30);
	_sizerVertical->Add(_btnCurrDate);
	_sizerHorizontal->AddSpacer(30);
	// schedule grid
	_sizerHorizontal->Add(_grid, 1, wxEXPAND);
	_sizerHorizontal->Layout();
	_currentDate = asDateModel(firstNow);

	refreshInternal();
}

MainWindow::~MainWindow() {
	_app->setMainWindow(nullptr);
	_sizerVertical->Clear(true);
}

void MainWindow::updateDate() {
	// displayed data update
	_textCurrentDate->SetValue(wxString::Format(
		wxT("Current Day\n%i.%i.%i"), _currentDate.getDay(), _currentDate.getMonth(), _currentDate.getYear()));
}

void MainWindow::refresh() {
	wxMutexGuiEnter();
	refreshInternal();
	wxMutexGuiLeave();
}

void MainWindow::refreshInternal() {
	// update permissions
	checkPermissions();

	checkJobs();
	// drawing table
	drawTable();
	// date update
	updateDate();
}

void MainWindow::openShiftEditor(identity_t shiftId) {
	auto p = _app->getClient().getPermissions();
	auto canEdit = (p & UserPermissions::Edit) != UserPermissions::None;
	if (!canEdit)
		return;
	if (!_app->getShiftWindow()) {
		auto wnd = new ShiftEditorWindow(_app, shiftId);
		wnd->Show();
		_app->setShiftWindow(wnd);
	}
	_app->getShiftWindow()->SetFocus();
}

void MainWindow::checkJobs() {
	currentJobs.clear();
	// collects unique jobs from a given day
	for (auto ref : _app->getClient().getShifts(_currentDate)) {
		auto& shift = ref.get();
		currentJobs.insert(shift.getJobName());
	}
}

void MainWindow::resizeGrid(int nRows, int nCols) {
	int rows = _grid->GetNumberRows();
	int cols = _grid->GetNumberCols();
	if (nRows > rows) {
		_grid->AppendRows(nRows - rows);
	}
	else {
		_grid->DeleteRows(0, rows - nRows);
	}

	if (nCols > cols) {
		_grid->AppendCols(nCols - cols);
	}
	else {
		_grid->DeleteCols(0, cols - nCols);
	}
}

void MainWindow::drawTable() {
	// clear the grid and reset cell sizes
	_grid->ClearGrid();
	_gridShifts.clear();

	for (int row = 0; row < _grid->GetNumberRows(); row++) {
		for (int col = 0; col < _grid->GetNumberCols(); col++) {
			_grid->SetCellSize(row, col, 1, 1);
			_grid->SetBackgroundColour(wxColour(255, 255, 255));
		}
	}
	// add/remove rows
	resizeGrid(HOUR_COUNT, std::max(1u, currentJobs.size()));


	
	// update hour labels
	for (int row = 0, hour = START_HOUR; row < HOUR_COUNT; row++, hour++) {
		_grid->SetRowLabelValue(row, std::to_string(hour) + ":00" + " - " + std::to_string(hour + 1) + ":00");
	}
	_grid->SetColLabelValue(0, "No data");

	std::map<std::wstring, int> jobToColumn;
	{
		int col = 0;
		for (auto job : currentJobs) {
			_grid->SetColLabelValue(col, job);
			jobToColumn[job] = col++;
		}
	}

	// set shifts
	
	auto shifts = _app->getClient().getShifts(_currentDate);

	for (auto ref : shifts) {
		const auto& shift = ref.get();
		std::wstring workerName = L"-=EMPTY=-";
		try {
			const auto& worker = _app->getClient().getWorker(shift.getWorkerId());
			workerName = worker.getFirstName() + " " + worker.getLastName();
		}
		catch (std::exception&) { }

		int col = jobToColumn[shift.getJobName()];
		int row = shift.getStartTime().getHour() - START_HOUR;

		_grid->SetCellValue(row, col, workerName);
		_grid->SetCellSize(row, col, shift.getWorkHours(), 1);


		_gridShifts[std::make_tuple(row, col)] = shift.getId();
	}

	_grid->AutoSize();
	_sizerHorizontal->Layout();
	//grid->Refresh();
}

void MainWindow::checkPermissions() {
	auto p = _app->getClient().getPermissions();
	auto none = UserPermissions::None;

	auto canInsert = (p & UserPermissions::Insert) != none;
	if (canInsert) {
		_btnEditData->Show();
	}
	else {
		_btnEditData->Hide();
	}
}

void MainWindow::onCurrDateButtonClicked(wxCommandEvent& evt) {
	const auto now = wxDateTime::Now();
	_calendar->SetValue(now);
	_currentDate = asDateModel(now);
	refreshInternal();
	evt.Skip();
}

void MainWindow::onRefreshButtonClicked(wxCommandEvent& evt) {
	refreshInternal();
	evt.Skip();
}


void MainWindow::onEditDataButtonClicked(wxCommandEvent& evt) {
	openShiftEditor(0);
	evt.Skip();
}

void MainWindow::onDateChanged(wxDateEvent& evt) {
	const auto& date = evt.GetDate();
	_currentDate = asDateModel(date);
	try {
		_app->getClient().queryShiftsByDay(_currentDate);
	}
	catch (std::exception& ex) {
		showExceptionMessageBox("Connection failure", "Error while connecting to the server", ex, this);
	}
	evt.Skip();
}

void MainWindow::onCellLeftClicked(wxGridEvent& evt) {
	// make all cells read-only
	evt.StopPropagation();
}

void MainWindow::onCellLeftDoubleClicked(wxGridEvent& evt) {
	auto pos = std::make_tuple(evt.GetRow(), evt.GetCol());
	auto it = _gridShifts.find(pos);
	if (it != _gridShifts.end()) {
		openShiftEditor(it->second);
		evt.Skip();
	}
}

void MainWindow::onSizeChanged(wxSizeEvent& evt) {
	_grid->AutoSize();
	_sizerVertical->Layout();
	evt.Skip();
}

void MainWindow::onShown(wxShowEvent& evt) {
	_app->setMainWindow(this);
	try {
		_app->getClient().queryShiftsByDay(_currentDate);
	}
	catch (std::exception& ex) {
		showExceptionMessageBox("Connection failure", "Error while connecting to the server", ex, this, true);
	}
}
