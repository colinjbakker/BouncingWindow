#pragma once

#include "wx/wx.h"

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

public:
	
	wxTimer* m_timer = nullptr;
	wxPanel* m_panel = nullptr;
	wxButton* close_button = nullptr;
	//wxTextCtrl* m_txt = nullptr;
	wxRealPoint pos;
	wxRealPoint dragStart; 
	double vx = 0.0;
	double vy = 0.0;
	double ax = 0.0;
	double ay = 0.0;
	double rotationAngle = 0.0;
	double angularV = 0.0;
	double angularA = 0.0;
	wxBitmap bitmaps[16];
	wxRegion regions[16];
	int currentBitmap = 0;

	double time = 0.0;


	int displayHeight = wxGetDisplaySize().GetHeight();
	int displayWidth = wxGetDisplaySize().GetWidth();


private:
	bool drag = false;
	void createBitmaps();
	void OnProgressTimer(wxTimerEvent& event);
	void OnClose(wxCommandEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	void UpdateBitmap();
	wxDECLARE_EVENT_TABLE();
};

