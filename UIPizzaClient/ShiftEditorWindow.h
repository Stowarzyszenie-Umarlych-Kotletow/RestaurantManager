#pragma once

#include "RestaurantClient.h"
#include "wx/wx.h"
#include "wx/frame.h"
#include "wx/calctrl.h"
#include "wx/datectrl.h"
#include "wx/choice.h"


class App;

// Shift Editor window
/**
 * ShiftEditorWindow class represents a Shift Editor window
 * Inherits from wxFrame class
 */
class ShiftEditorWindow final : public wxFrame {
private:
	App* _app; /**<Pointer to the App object*/
	identity_t _shiftId; /**<shift ID*/

	// window elements
	/**
	 * UI elements
	 */
	wxPanel* _panel = nullptr;
	wxTextCtrl* _textCtrlWorkerName = nullptr;
	wxTextCtrl* _textCtrlWorkerJob = nullptr;
	wxTextCtrl* _textCtrlHourStart = nullptr;
	wxTextCtrl* _textCtrlHourEnd = nullptr;
	wxTextCtrl* _textCtrlDate = nullptr;
	wxDatePickerCtrl* _calendar;
	wxChoice* _choiceStartHour;
	wxChoice* _choiceEndHour;
	wxChoice* _choiceWorker;
	wxStaticText* _textBoxTitle = nullptr;
	wxStaticText* _textBoxWorkerName = nullptr;
	wxStaticText* _textBoxWorkerJob = nullptr;
	wxStaticText* _textBoxHourStart = nullptr;
	wxStaticText* _textBoxHourEnd = nullptr;
	wxStaticText* _textBoxDate = nullptr;
	wxButton* _buttonSubmit = nullptr;
	wxButton* _buttonDelete = nullptr;
	wxButton* _buttonEdit = nullptr;

	/**
	 * Method that loads shift into Shift Editor window
	 * @param shiftId - shift ID
	 */
	void loadShift(identity_t shiftId);

	/**
	 * Event handlers
	 */
	void onSubmitButtonClicked(wxCommandEvent& evt);
	void onChoice(wxCommandEvent& evt);
	void onDeleteButtonClicked(wxCommandEvent& evt);
	void onEditButtonClicked(wxCommandEvent& evt);
	void onStartHourChoice(wxCommandEvent& evt);
	/**
	 * Updates the end hours wxChoice
	 */
	void updateEndHourChoice();

	/**
	 * Methods that refreshes current window
	 */
	void refreshInternal();
	/**
	 * Method that opens Worker Editor window
	 * @param id - worker ID (default 0)
	 */
	void openWorkerEditor(identity_t id = 0);
public:
	/**
	 * ShiftEditorWindow contructor
	 * @param app - App object pointer
	 * @param shiftId - shift ID (default 0)
	 */
	ShiftEditorWindow(App* app, identity_t shiftId = 0);
	/**
	 * ShiftEditorWindow destructor
	 */
	~ShiftEditorWindow();
public:

	/**
	 * Methods that refreshes every window in the current context
	 */
	void refresh();

	/**
	 * Methods that converts worker wstring into ShiftWorker object
	 * @param input - wstring with worker data
	 * @returns ShiftWorker object containing worker's data
	 */
	ShiftWorker getWorkerData(std::wstring input);
	void onShiftInserted(const S2C_InsertShiftReply& reply);
	void onShiftDeleted(const S2C_DeleteShiftReply& reply);

wxDECLARE_EVENT_TABLE();
};

#include "App.h"
