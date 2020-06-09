#pragma once
#include <wx/generic/gridctrl.h>
#include "Shift.h"
#include "ShiftWorker.h"

/**
 * ShiftCellRenderer class represents set of settings for beautiful UI layout
 * Inherits from wxGridCellStringRenderer
 */
class ShiftCellRenderer : public wxGridCellStringRenderer {
protected:
	Shift _shift; /**<current shift obj*/
	ShiftWorker _worker; /**<current shiftWorker obj*/
public:
	/**
	 * ShiftCellRenderer constructor
	 */
	ShiftCellRenderer() : wxGridCellStringRenderer() { }

	/**
	 * Method that does some magic on the grid cell to make it more appealing
	 * @param grid wxGrid obj
	 * @param attr wxGridCellAttr obj
	 * @param dc wxDC obj
	 * @param rect wxRect obj
	 * @param row row index
	 * @param col col index
	 * @param isSelected bool if sth is selected xD
	 */
	void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col,
	          bool isSelected) override {
		//wxGridCellStringRenderer::Draw(grid, attr, dc, rect, row, col, isSelected);
		wxGridCellRenderer::Draw(grid, attr, dc, rect, row, col, isSelected);
		const auto& text = grid.GetCellValue(row, col);
		auto bg = wxBrush(wxColour(255, 255, 255));
		if (text != "") {
			bg = wxBrush(wxColour(0x69, 0xf0, 0xae));
		}
		dc.SetBrush(bg);
		dc.SetPen(wxPen(*wxLIGHT_GREY, 1, wxPENSTYLE_SOLID));
		dc.DrawRectangle(rect);
		dc.SetTextBackground(bg.GetColour());
		grid.DrawTextRectangle(dc, text, rect, wxALIGN_CENTER, wxALIGN_CENTER);
	}

	/**
	 * Method that tries to get the best size for the grid to fit the currecn window size
	 * @param grid wxGrid obj
	 * @param attr wxGridCellAttr obj
	 * @param dc wxDC obj
	 * @param row row index
	 * @param col col index
	 */
	wxSize GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, int row, int col) override {
		auto rect = grid.GetClientRect();
		int minHeightPerCell = std::max(0, (rect.GetHeight() - 128)) / grid.GetNumberRows();
		int rows, cols;
		auto sz = grid.GetCellSize(row, col, &rows, &cols);
		auto size = DoGetBestSize(attr, dc, grid.GetCellValue(row, col));
		return wxSize(std::max(80, size.GetWidth()), std::max(minHeightPerCell * rows, size.GetHeight()));
	}

	/**
	 * @returns Shift obj
	 */
	virtual Shift& getShift() {
		return _shift;
	}

	/**
	 * @param shift - Shift obj
	 */
	virtual void setShift(Shift shift) {
		_shift = shift;
	}

	/**
	 * @returns ShiftWorker obj
	 */
	virtual ShiftWorker& getWorker() {
		return _worker;
	}

	/**
	 * @param worker - ShiftWorker obj
	 */
	virtual void setWorker(ShiftWorker worker) {
		_worker = worker;
	}
};
