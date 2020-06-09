#pragma once
#include "wx/wx.h"
#include "Connection.h"
#include "S2C_AuthorizeReply.h"
#include "S2C_DeleteShiftReply.h"
#include "S2C_DeleteWorkerReply.h"
#include "S2C_GetShiftsReply.h"
#include "S2C_GetWorkersReply.h"
#include "S2C_InsertShiftReply.h"
#include "S2C_InsertWorkerReply.h"
#include "RestaurantClient.h"

const std::string HOST = "127.0.0.1";
const int PORT = 1337;
const std::string ACCESS_TOKEN = "melon";

const int START_HOUR = 8;
const int END_HOUR = 20;
const int HOUR_COUNT = END_HOUR - START_HOUR + 1;

class MainWindow;
class ShiftEditorWindow;
class WorkerEditorWindow;

/**
 * App class represents the base of everything
 * Inherits from wxApp and ConnectionObserver classes
 */
class App : public wxApp, public ConnectionObserver {
public:
	/**
	 * App class constructor
	 */
	App();
	/**
	 * App class destructor
	 */
	~App();
private:
	MainWindow* _window = nullptr; /**<Main app window pointer */
	ShiftEditorWindow* _shiftWindow = nullptr; /**<Shift Editor window pointer*/
	WorkerEditorWindow* _workerWindow = nullptr; /**<Worker Editor window pointer*/
	RestaurantClient _client;
protected:
	/* Networking stuff */
	
	void onAuthorize(ConnectionBase* connection, const S2C_AuthorizeReply& payload, size_t size);
	void onDeleteShift(ConnectionBase* connection, const S2C_DeleteShiftReply& payload, size_t size);
	void onGetShiftsByDay(ConnectionBase* connection, const S2C_GetShiftsReply& payload, size_t size);
	void onGetWorkers(ConnectionBase* connection, const S2C_GetWorkersReply& payload, size_t size);
	void onInsertShift(ConnectionBase* connection, const S2C_InsertShiftReply& payload, size_t size);
	void onInsertWorker(ConnectionBase* connection, const S2C_InsertWorkerReply& payload, size_t size);
	void onDeleteWorker(ConnectionBase* connection, const S2C_DeleteWorkerReply& payload, size_t size);
	void onSync(ConnectionBase* connection, const S2C_ClientSync& payload, size_t size);
	void onConnected(ConnectionBase* connection) override;

public:
	/**
	 * Method that initializes the main window
	 */
	bool OnInit() override;

	/**
	 * @returns current client connection
	 */
	RestaurantClient& getClient() {
		return _client;
	}

	/**
	 * @returns main window pointer
	 */
	MainWindow* getMainWindow() const {
		return _window;
	}

	/**
	 *@returns Shift Editor window pointer
	 */
	ShiftEditorWindow* getShiftWindow() const {
		return _shiftWindow;
	}

	/**
	 * @returns Worker Editor window pointer
	 */
	WorkerEditorWindow* getWorkerWindow() const {
		return _workerWindow;
	}

	/**
	 * Method which refreshes every window in the current context
	 */
	void refreshUi();

	/**
	 * Method that sets Main Window pointer
	 * @param wnd - Main Window pointer
	 */
	void setMainWindow(MainWindow* wnd) {
		_window = wnd;
	}

	/**
	 * Method that sets Main Window pointer
	 * @param wnd - Shift Editor window pointer
	 */
	void setShiftWindow(ShiftEditorWindow* wnd) {
		_shiftWindow = wnd;
	}

	/**
	 * Method that sets Main Window pointer
	 * @param wnd - Worker Editor window pointer
	 */
	void setWorkerWindow(WorkerEditorWindow* wnd) {
		_workerWindow = wnd;
	}
};

#include "MainWindow.h"
#include "ShiftEditorWindow.h"
#include "WorkerEditorWindow.h"
