#include "App.h"
#include <future>
#include "BaseLibrary.h"
#include "MainWindow.h"
#include "utils.h"


//macro definition
wxIMPLEMENT_APP(App);

App::App() {
	setlocale(LC_ALL, "en_US.UTF-8");
	InitializeBaseLibrary();
	_client.getConnection().subscribe(this);
	addHandler(&App::onAuthorize);
	addHandler(&App::onDeleteShift);
	addHandler(&App::onGetShiftsByDay);
	addHandler(&App::onGetWorkers);
	addHandler(&App::onInsertShift);
	addHandler(&App::onInsertWorker);
	addHandler(&App::onDeleteWorker);
	addHandler(&App::onSync);
}

App::~App() {
	ShutdownBaseLibrary();
	Exit();
}

bool App::OnInit() {
	// configure connection settings
	_client.setEndpoint(HOST, PORT);
	_client.setToken(ACCESS_TOKEN);
	try {
		_client.connect();
	}
	catch (std::exception& ex) {
		showExceptionMessageBox("Connection failure", "Error while contacting the server", ex);
		Exit();
		return false;
	}
	_window = new MainWindow(this);
	// displaying UI frame 
	_window->Show();
	return true;
}

void App::refreshUi() {
	if (_window)
		_window->refresh();
	if (_shiftWindow)
		_shiftWindow->refresh();
}

void App::onAuthorize(ConnectionBase* connection, const S2C_AuthorizeReply& payload, size_t size) {
	refreshUi();
}

void App::onDeleteShift(ConnectionBase* connection, const S2C_DeleteShiftReply& payload, size_t size) {
	if (getShiftWindow())
		getShiftWindow()->onShiftDeleted(payload);
	if (payload.isSuccess())
		refreshUi();
}

void App::onGetShiftsByDay(ConnectionBase* connection, const S2C_GetShiftsReply& payload, size_t size) {
	if (payload.getDate() == _window->getCurrentDate()) {
		refreshUi();
	}
}

void App::onGetWorkers(ConnectionBase* connection, const S2C_GetWorkersReply& payload, size_t size) {
	if (payload.isSuccess()) {
		refreshUi();
	}
	else {
		showTransactionMessageBox("Worker list", "Retrieving worker list failed", payload);
	}
}

void App::onInsertShift(ConnectionBase* connection, const S2C_InsertShiftReply& payload, size_t size) {
	if (getShiftWindow())
		getShiftWindow()->onShiftInserted(payload);
	if (payload.isSuccess()) {
		refreshUi();
	}
}

void App::onInsertWorker(ConnectionBase* connection, const S2C_InsertWorkerReply& payload, size_t size) {
	if (getWorkerWindow())
		getWorkerWindow()->onWorkerInserted(payload);
	if (payload.isSuccess()) {
		refreshUi();
	}
}

void App::onDeleteWorker(ConnectionBase* connection, const S2C_DeleteWorkerReply& payload, size_t size) {
	if (getWorkerWindow())
		getWorkerWindow()->onWorkerDeleted(payload);
	if (payload.isSuccess()) {
		refreshUi();
	}
}

void App::onSync(ConnectionBase* connection, const S2C_ClientSync& payload, size_t size) {
	refreshUi();
}

void App::onConnected(ConnectionBase* connection) {
	ConnectionObserver::onConnected(connection);
	try {
		_client.queryWorkers();
		if (_window)
			_client.queryShiftsByDay(_window->getCurrentDate());
	} catch(std::exception&) {}
}
