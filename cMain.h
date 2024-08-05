#pragma once

#include "wx/wx.h"

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

public:
	double time = 0.0;
	wxTimer* m_timer = nullptr;
	wxPanel* m_panel = nullptr;
	wxButton* close_button = nullptr;
	//wxTextCtrl* m_txt = nullptr;
	wxRealPoint pos;
	wxPoint dragStart;
	double vx = 0.0;
	double vy = 0.0;
	double ax = 0.0;
	double ay = 0.0;
private:
	bool drag = false;
	void OnProgressTimer(wxTimerEvent& event);
	void OnClose(wxCommandEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	wxDECLARE_EVENT_TABLE();
};

