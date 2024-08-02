#include "cMain.h"

#define GRAVITY 0.15
#define INTERVAL 10
#define FRICTION 0.1
#define BOUNCE 0.5

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_TIMER(10001, cMain::OnProgressTimer)
	EVT_BUTTON(10002, cMain::OnClose)
wxEND_EVENT_TABLE()

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "", wxPoint(400,30), wxSize(200, 200), wxSTAY_ON_TOP)
{
	m_timer = new wxTimer(this, 10001);
	m_panel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(200, 200));
	close_button = new wxButton(m_panel, 10002, "X", wxPoint(145, 2), wxSize(50, 20));

	pos = wxRealPoint(400.0, 30.0);
	m_panel->Bind(wxEVT_LEFT_DOWN, &cMain::OnMouseDown, this);
	m_panel->Bind(wxEVT_LEFT_UP, &cMain::OnMouseUp, this);
	m_panel->Bind(wxEVT_MOTION, &cMain::OnMouseMove, this);
	time = wxGetLocalTimeMillis().ToDouble();
	vx = -4.0;
	vy = 0.0;
	ax = 0.0;
	ay = GRAVITY;
	m_timer->Start(INTERVAL);
}

cMain::~cMain() {

}

void cMain::OnClose(wxCommandEvent& event) {
	m_timer->Stop();
	delete m_timer;
	Close();
}

void cMain::OnProgressTimer(wxTimerEvent& event) 
{
	double dt = (wxGetLocalTimeMillis() - time).ToDouble() / 10.0;
	time = wxGetLocalTimeMillis().ToDouble();

	pos.x = this->GetScreenPosition().x + (vx * dt);
	pos.y = this->GetScreenPosition().y + (vy * dt);
	vx = vx > 0 ? std::max(vx + (ax * dt), 0.0) : std::min(vx + (ax * dt), 0.0);
	vy = vy + (ay * dt);
	int h = this->GetSize().GetHeight();
	int w = this->GetSize().GetWidth();

	if (pos.y > wxGetDisplaySize().GetHeight() - h || pos.y < 0) {
		ay = pos.y < 0 ? GRAVITY : 0;
		if (vx == 0) {
			ax = 0;
		}
		else {
			ax = (vx > 0) ? -FRICTION : FRICTION;
		}
		vy = (vy > 0 && vy < 2) || (vy < 0 && vy > -2) ? 0 : - vy * BOUNCE;
		pos.y = pos.y < 0 ? 0 : wxGetDisplaySize().GetHeight() - h;
	}
	else {
		ax = 0;
		ay = GRAVITY;
	}

	if (pos.x > wxGetDisplaySize().GetWidth() - w || pos.x < 0) {
		vx = -vx * BOUNCE;
		ax = -ax;
		pos.x = (pos.x < 0) ? 0 : wxGetDisplaySize().GetWidth() - w;
	}
	this->Move(pos);
	event.Skip();
}

void cMain::OnMouseMove(wxMouseEvent& event) {
	if (event.Dragging()) {
		wxRealPoint newPos = ClientToScreen(event.GetPosition() - dragStart);
		wxPoint diff = wxPoint(newPos.x - pos.x, newPos.y - pos.y);
		pos = newPos;
		this->Move(pos);
		ax = 0;
		ay = 0;
		vx = diff.x;
		vy = diff.y;
	}
}

void cMain::OnMouseDown(wxMouseEvent& event) {
	if (m_timer->IsRunning()) {
		m_timer->Stop();
	}
	dragStart = event.GetPosition();
	event.Skip();
}

void cMain::OnMouseUp(wxMouseEvent& event) {
	if (!(m_timer->IsRunning())) {
		time = wxGetLocalTimeMillis().ToDouble();
		m_timer->Start();
	}
	event.Skip();
}