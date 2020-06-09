#pragma once
#include "RestaurantClient.h"
#include "wx/wx.h"
#include "wx/frame.h"
#include "wx/calctrl.h"
#include "wx/datectrl.h"
#include "wx/choice.h"


class App;

// Worker Editor window
/**
 * WorkerEditorWindow class represents the Worker Editor window
 * Inherits from the wxFrame class
 */
class WorkerEditorWindow final : public wxFrame {
private:
	App* _app; /**<App object pointer*/
	identity_t _workerId; /**<ID of the worker*/

	/**
	 * UI elements
	 */
	wxPanel* _panel = nullptr;
	wxTextCtrl* _textCtrlWorkerFirstName = nullptr;
	wxTextCtrl* _textCtrlWorkerLastName = nullptr;
	wxTextCtrl* _textCtrlWorkerTitle = nullptr;

	wxStaticText* _textBoxTitle = nullptr;
	wxStaticText* _textBoxWorkerFirstName = nullptr;
	wxStaticText* _textBoxWorkerLastName = nullptr;
	wxStaticText* _textBoxWorkerTitle = nullptr;

	wxButton* _buttonSubmit = nullptr;
	wxButton* _buttonDelete = nullptr;


public:
	/**
	 * WorkerEditorWindow constructor
	 * @param app - App object pointer
	 * @param workerId - ID of the chosen worker
	 */
	WorkerEditorWindow(App* app, identity_t workerId = 0);
	/**
	 * WorkerEditorWindow destructor
	 */
	~WorkerEditorWindow();


	/*
	 * Event handlers
	 */
	void onSubmitButtonClicked(wxCommandEvent& evt);
	void onDeleteButtonClicked(wxCommandEvent& evt);
	void onWorkerInserted(const S2C_InsertWorkerReply& reply);
	void onWorkerDeleted(const S2C_DeleteWorkerReply& reply);

	// all events table
wxDECLARE_EVENT_TABLE();
};

#include "App.h"
