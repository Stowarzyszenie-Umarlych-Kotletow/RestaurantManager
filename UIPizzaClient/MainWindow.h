#pragma once
#include <vector>
#include <set>
#include "wx/wx.h"
#include "wx/frame.h"
#include "Shift.h"
#include "wx/calctrl.h"
#include "wx/datectrl.h"
#include "wx/grid.h"

class App;
class ShiftEditorWindow;

// Main window frame
/**
 * MainWindow class represents the main window of the app
 * Inherits from wxFrame class
 */
class MainWindow final : public wxFrame {
private:
	App* _app; /**<Pointer to the base App*/
	Date _currentDate; /**<Keeps current date as Date*/
	std::map<std::tuple<int, int>, identity_t> _gridShifts; /**<Keeps shifts ID by x and y positions*/

	std::set<std::wstring> currentJobs; /**<Keeps jobs for the current day*/

	/**
	 * UI elements
	 */
	wxGrid* _grid;
	wxBoxSizer* _sizerHorizontal;
	wxBoxSizer* _sizerVertical;
	// buttons declaration
	wxButton* _btnRefresh = nullptr;
	wxButton* _btnNextDay = nullptr;
	wxButton* _btnPreviousDay = nullptr;
	wxButton* _btnEditData = nullptr;
	wxButton* _btnCurrDate = nullptr;
	wxTextCtrl* _textCurrentDate = nullptr;
	wxDatePickerCtrl* _calendar = nullptr;

	/**
	 * Method that refreshes current UI window
	 */
	void refreshInternal();
	/**
	 * Method that opens Shift Editor window with set shiftId
	 * @param shiftId - id of the chosen shift
	 */
	void openShiftEditor(identity_t shiftId);
public:
	/**
	 * MainWindow constructor
	 * @param app - App object pointer
	 */
	MainWindow(App* app);
	/**
	 * MainWindow destructor
	 */
	~MainWindow();

	/**
	 * @returns _currentDate - chosen date
	 */
	Date& getCurrentDate() {
		return _currentDate;
	}


	/**
	 * Methods that sets a new date
	 */
	void updateDate();
	/**
	 * Methods that refreshes the main window
	 */
	void refresh();
	/**
	 * Method that checks for current jobs
	 */
	void checkJobs();
	/**
	 * Methods that sets grid size
	 * @param nRows - number of rows
	 * @param nCols - number of columns
	 */
	void resizeGrid(int nRows, int nCols);
	/**
	 * Method that overwrites the current grid with needed data
	 */
	void drawTable();
	void checkPermissions();

	/**
	 * Event handlers
	 */
	void onCurrDateButtonClicked(wxCommandEvent& evt);
	void onRefreshButtonClicked(wxCommandEvent& evt);
	void onEditDataButtonClicked(wxCommandEvent& evt);
	void onDateChanged(wxDateEvent& evt);
	void onCellLeftClicked(wxGridEvent& evt);
	void onCellLeftDoubleClicked(wxGridEvent& evt);;
	void onSizeChanged(wxSizeEvent& evt);
	void onShown(wxShowEvent& evt);

	//all events table
wxDECLARE_EVENT_TABLE();
};

#include "App.h"
#include "ShiftEditorWindow.h"
