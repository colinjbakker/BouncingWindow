#include "cMain.h"

unsigned const int INTERVAL = 5;
const float GRAVITY = 0.2;
const float FRICTION = 0.25;
const float BOUNCINESS = 0.5;

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
	if (drag && wxGetMouseState().LeftIsDown()) {
		wxPoint newPos = wxGetMousePosition() - dragStart;
		wxPoint diff = wxPoint(newPos.x - pos.x, newPos.y - pos.y);
		pos = newPos;
		this->Move(pos);
		ax = 0;
		ay = 0;
		vx = diff.x;
		vy = diff.y;
	}
	else {
		drag = false;

		const unsigned int floorY = wxGetDisplaySize().GetHeight() - this->GetSize().GetHeight();
		const unsigned int wallX = wxGetDisplaySize().GetWidth() - this->GetSize().GetWidth();

		pos.x = this->GetScreenPosition().x + (vx * dt);
		pos.y = this->GetScreenPosition().y + (vy * dt);

		vx = vx > 0 ? std::max(vx + (ax * dt), 0.0) : std::min(vx + (ax * dt), 0.0);
		vy = vy + (ay * dt);
		

		ax = 0;
		ay = GRAVITY;

		if (pos.y >= floorY || pos.y < 0) {
			//wxLogDebug("before: " + wxString::Format(wxT("%f"), vy));
			ay = (pos.y < 0) ? GRAVITY : 0;
			ax = (vx == 0) ? 0 : (vx > 0) ? -FRICTION : FRICTION;
			vy = (vy > -2 && vy < 2) ? 0 : - (vy - GRAVITY * 2 * dt) * BOUNCINESS;
			pos.y = (pos.y < 0) ? 0 : floorY;
			//wxLogDebug("after: " + wxString::Format(wxT("%f"), vy));
		}

		if (pos.x >= wallX || pos.x <= 0) {
			vx = -vx * BOUNCINESS;
			ax = -ax;
			pos.x = (pos.x <= 0) ? 0 : wallX;
		}
		this->Move(pos);
	}
	
	event.Skip();
}

void cMain::OnMouseDown(wxMouseEvent& event) {
	dragStart = event.GetPosition();
	drag = true;
	event.Skip();
}
