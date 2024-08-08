#include "cMain.h"

#define PI 3.14159265358979323846

unsigned const int INTERVAL = 10;
const float GRAVITY = 0.2;
const float FRICTION = 0.25;
const float BOUNCINESS = 0.5;
const float GRAVITYANGULAR = 0.05;

const wxRealPoint GRAVITYVECTOR = wxRealPoint(0.0, 0.2);

float angleDifference = 0.0;

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_TIMER(10001, cMain::OnProgressTimer)
	EVT_BUTTON(10002, cMain::OnClose)
wxEND_EVENT_TABLE()

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "", wxPoint(400,30), wxSize(200, 200), wxSTAY_ON_TOP | wxFRAME_SHAPED)
{
	createBitmaps();
	
	m_timer = new wxTimer(this, 10001);
	m_panel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(200, 200));
	m_panel->SetBackgroundColour(*wxRED);
	close_button = new wxButton(m_panel, 10002, "X", wxPoint(90, 90), wxSize(20, 20));
	pos = wxRealPoint(400.0, 30.0);
	m_panel->Bind(wxEVT_LEFT_DOWN, &cMain::OnMouseDown, this);
	time = wxGetLocalTimeMillis().ToDouble();
	
	vx = -4.0;
	vy = 0.0;
	ax = 0.0;
	ay = GRAVITY;
	angularA = 0.0;
	angularV = 0.0;

	rotationAngle = 44;
	currentBitmap = 8;
	SetShape(regions[8]);
	m_timer->Start(INTERVAL);
}

cMain::~cMain() {

}

void cMain::createBitmaps() {
	/*
	(-100cos(a)-0sin(a)+100,-100sin(a)+0cos(a)+100)
	(0cos(a)-100sin(a)+100,0sin(a)+100cos(a)+100)
	(100cos(a)-0sin(a)+100,100sin(a)+0cos(a)+100)
	(0cos(a)+100sin(a)+100,0sin(a)-100cos(a)+100)
	*/
	wxMemoryDC dc;
	wxPoint points[4];
	float angle = 0;
	dc.SetBrush(*wxGREEN_BRUSH);
	for (int i = 0; i < 16; i++){
		points[0] = wxPoint(floor(-100 * cos(angle) + 100), floor(-100 * sin(angle) + 100));
		points[1] = wxPoint(floor(-100 * sin(angle) + 100), floor(100 * cos(angle) + 100));
		points[2] = wxPoint(floor(100 * cos(angle) + 100), floor(100 * sin(angle) + 100));
		points[3] = wxPoint(floor(100 * sin(angle) + 100), floor(-100 * cos(angle) + 100));
		bitmaps[i] = wxBitmap(200, 200);
		dc.SelectObject(bitmaps[i]);
		dc.DrawPolygon(4, points);
		regions[i] = wxRegion(bitmaps[i], *wxBLACK);
		angle += PI / 32;
	}
}

void cMain::OnClose(wxCommandEvent& event) {
	m_timer->Stop();
	delete m_timer;
	Close();
}
float diffsum = 0.0;
void cMain::OnProgressTimer(wxTimerEvent& event) 
{
	double dt = (wxGetLocalTimeMillis() - time).ToDouble() / 10.0;
	time = wxGetLocalTimeMillis().ToDouble();

	angularV += angularA * dt;
	rotationAngle += angularV * dt;
	rotationAngle = fmod(rotationAngle, 360);
	if (rotationAngle < 0) {
		rotationAngle += 360;
	}
	UpdateBitmap();

	if (drag && wxGetMouseState().LeftIsDown()) {
		wxRealPoint vCenterToHand(wxGetMousePosition().x - GetScreenPosition().x - 100, -(wxGetMousePosition().y - GetScreenPosition().y - 100));
		
		angleDifference += angularV * dt;
		float newX = (100 - dragStart.x) * cos(-angleDifference * (PI / 180)) - (100 - dragStart.y) * sin(-angleDifference * (PI / 180)) + dragStart.x;
		float newY = (100 - dragStart.x) * sin(-angleDifference * (PI / 180)) + (100 - dragStart.y) * cos(-angleDifference * (PI / 180)) + dragStart.y;

		wxPoint newPos = wxPoint(wxGetMousePosition().x - dragStart.x + newX - 100, wxGetMousePosition().y - dragStart.y + newY - 100);
		wxPoint diff = wxPoint(newPos.x - pos.x, newPos.y - pos.y);

		vx = diff.x;
		vy = diff.y;

		//force of gravity
		//force of air resistance
		//torque = radius * force * sin(angle)
		//force of air resistance = c * v ^ 2
		//radius = ||vCenterToHand||
		//air resistance vector = -diff
		//double theta = acos(-vCenterToHand.y / sqrt(pow(vCenterToHand.x, 2) + pow(vCenterToHand.y, 2)));
		float radius = sqrt(vCenterToHand.x * vCenterToHand.x + vCenterToHand.y * vCenterToHand.y);
		float gravityTorque = GRAVITY * vCenterToHand.x / 100;
		//double airtheta = (sqrt(vx * vx + vy * vy) < 2) ? 0 : acos(vCenterToHand.x * vx * -1 + vCenterToHand.y * vy * -1 / (radius * sqrt(vx * vx + vy * vy)));
		float airTorque = 0.02 * (-vx * vCenterToHand.y + vy * -vCenterToHand.x) / 100;
		angularA = gravityTorque + airTorque;
		//wxLogDebug("angularA: " + wxString::Format(wxT("%f"), angularA) + " gravityTorque: " + wxString::Format(wxT("%f"), gravityTorque) + " airTorque: " + wxString::Format(wxT("%f"), airTorque));

		pos = newPos;
		this->Move(pos);
		ax = 0;
		ay = 0;
		
		/*
		if (theta < 31 * PI / 32) {
			angularA = (vCenterToHand.x > 0) ? GRAVITYANGULAR * sin(theta) : -GRAVITYANGULAR * sin(theta);
			if ((angularA < 0) == (angularV < 0)) {
				angularA *= 0.8;
			}
			else {
				angularA *= 1.2;
			}
		}
		else {
			angularA = 0;
		}
		wxLogDebug("angularA: " + wxString::Format(wxT("%f"), angularA) + " , angularV: " + wxString::Format(wxT("%f"), dragStart.x) + " , theta: " + wxString::Format(wxT("%f"), theta));
		*/
		

		
	}
	else {
		angleDifference = 0.0;
		drag = false;

		//space between visual edge of box and actual edge of window
		double offset = (currentBitmap < 8) ? -100 * cos(currentBitmap * PI / 32) + 100 : -100 * sin(currentBitmap * PI / 32) + 100;
		
		const unsigned int floorY = displayHeight - bitmaps[currentBitmap].GetHeight() + offset;
		const unsigned int wallX = displayWidth - bitmaps[currentBitmap].GetWidth() + offset;

		pos.x = this->GetScreenPosition().x + (vx * dt);
		pos.y = this->GetScreenPosition().y + (vy * dt);

		vx = vx > 0 ? std::max(vx + (ax * dt), 0.0) : std::min(vx + (ax * dt), 0.0);
		vy = vy + (ay * dt);
		
		ax = 0;
		ay = GRAVITY;

		if (pos.y >= floorY || pos.y < 0 - offset) {
			//floor and ceiling
			//wxLogDebug("before: " + wxString::Format(wxT("%f"), vy));
			ay = (pos.y < 0 - offset) ? GRAVITY : 0;
			ax = (vx == 0) ? 0 : (vx > 0) ? -FRICTION : FRICTION;
			
			vx -= angularV * dt / 10;

			vy = (vy > -2 && vy < 2) ? 0 : - (vy - GRAVITY * 2 * dt) * BOUNCINESS;
			pos.y = (pos.y < 0 - offset) ? 0 - offset : floorY;

			//linear friction
			//angular friction
			//gravity angular

			float angularFrictionAngle = rotationAngle - floor((rotationAngle - 45)/ 90) * 90;

			float angularFrictionForce = -angularV * 0.05;

			float angularFrictionTorque = angularFrictionForce * sin(angularFrictionAngle * PI / 180);

			float gravityAngle = fmod(rotationAngle, 90);

			gravityAngle = (gravityAngle < 44) ? gravityAngle : 360 - gravityAngle;

			float gravityTorque = GRAVITYANGULAR * 2 * sin(gravityAngle * PI / 180);

			if ((angularV > 0) == (gravityTorque > 0)) {
				gravityTorque *= 0.5;
			}

			//wxLogDebug("angularFrictionTorque: " + wxString::Format(wxT(" % f"), angularFrictionTorque) + " gravityTorque: " + wxString::Format(wxT(" % f"), gravityTorque));

			//wxLogDebug("rotationAngle: " + wxString::Format(wxT(" % f"), rotationAngle) + " gravityAngle: " + wxString::Format(wxT(" % f"), gravityAngle) + " gravityTorque: " + wxString::Format(wxT(" % f"), gravityTorque));
			

			angularA = angularFrictionTorque + gravityTorque;
		}

		if (pos.x >= wallX || pos.x <= 0 - offset) {
			//walls
			vx = -vx * BOUNCINESS;
			ax = -ax;
			pos.x = (pos.x <= 0 - offset) ? 0 - offset : wallX;
		}

		if (pos.y < floorY && pos.y > 0 - offset && pos.x < wallX && pos.x > 0 - offset) {
			//in da air
			angularA = 0.0;
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
//int count = 0;
void cMain::UpdateBitmap() {
	float bitmapAngle = fmod(rotationAngle, 90);
	int newBitmap = 15 - floor(bitmapAngle / (90.0 / 16.0));
	//wxLogDebug("bitmap: " + wxString::Format(wxT("%i"), newBitmap));
	if (newBitmap >= 0 && newBitmap < 16 && newBitmap != currentBitmap) {
		currentBitmap = newBitmap;
		SetShape(regions[currentBitmap]);
	}
	/*
	if ((bitmapAngle >= 15 * PI / 32 && bitmapAngle < PI / 2) || (bitmapAngle >= 0 && bitmapAngle < PI / 32)) {
		newBitmap = 7;
	}
	else if (bitmapAngle >= PI / 32 && bitmapAngle < 3 * PI / 32) {
		newBitmap = 6;
	}
	else if (bitmapAngle >= 3 * PI / 32 && bitmapAngle < 5 * PI / 32) {
		newBitmap = 5;
	}
	else if (bitmapAngle >= 5 * PI / 32 && bitmapAngle < 7 * PI / 32) {
		newBitmap = 4;
	}
	else if (bitmapAngle >= 7 * PI / 32 && bitmapAngle < 9 * PI / 32) {
		newBitmap = 3;
	}
	else if (bitmapAngle >= 9 * PI / 32 && bitmapAngle < 11 * PI / 32) {
		newBitmap = 2;
	}
	else if (bitmapAngle >= 11 * PI / 32 && bitmapAngle < 13 * PI / 32) {
		newBitmap = 1;
	}
	else if (bitmapAngle >= 13 * PI / 32 && bitmapAngle < 15 * PI / 32) {
		newBitmap = 0;
	}
	*/
	


	/*





	//returns true if bitmap changes
	int newBitmap = -1;
	//wxLogDebug("count: " + wxString::Format(wxT("%i"), count));
	angularV += angularA * dt;
	rotationAngle += angularV * dt;
	//wxLogDebug("acc: " + wxString::Format(wxT("%f"), angularA) + " , vel: " + wxString::Format(wxT("%f"), angularV) + " , angle: " + wxString::Format(wxT("%f"), rotationAngle));
	//rotationAngle -= floor(rotationAngle / (PI / 2)) * (PI / 2);
	if (rotationAngle > PI / 16) {
		while (rotationAngle > PI / 16) {
			currentBitmap += 1;
			//count++;
			if (currentBitmap > 7) {
				currentBitmap = 0;
				
			}
			rotationAngle -= PI / 16;
		}
		rotationAngle = 0;
		SetShape(regions[currentBitmap]);
		return true;
	}
	else if (rotationAngle < -PI / 16){
		while (rotationAngle < -PI / 16) {
			currentBitmap -= 1;
			//count++;
			if (currentBitmap < 0) {
				currentBitmap = 7;
				
			}
			rotationAngle += PI / 16;
		}
		rotationAngle = 0;
		SetShape(regions[currentBitmap]);
		return true;
	}
	return false;

	

	
	if ((rotationAngle >= 15 * PI / 32 && rotationAngle < PI / 2) || (rotationAngle >= 0 && rotationAngle < PI / 32)) {
		newBitmap = 0;
	}
	else if (rotationAngle >= PI / 32 && rotationAngle < 3 * PI / 32) {
		newBitmap = 1;
	}
	else if (rotationAngle >= 3 * PI / 32 && rotationAngle < 5 * PI / 32) {
		newBitmap = 2;
	}
	else if (rotationAngle >= 5 * PI / 32 && rotationAngle < 7 * PI / 32) {
		newBitmap = 3;
	}
	else if (rotationAngle >= 7 * PI / 32 && rotationAngle < 9 * PI / 32) {
		newBitmap = 4;
	}
	else if (rotationAngle >= 9 * PI / 32 && rotationAngle < 11 * PI / 32) {
		newBitmap = 5;
	}
	else if (rotationAngle >= 11 * PI / 32 && rotationAngle < 13 * PI / 32) {
		newBitmap = 6;
	}
	else if (rotationAngle >= 13 * PI / 32 && rotationAngle < 15 * PI / 32) {
		newBitmap = 7;
	}

	if (newBitmap >= 0 && newBitmap < 8 && newBitmap != currentBitmap) {
		currentBitmap = newBitmap;
		SetShape(regions[currentBitmap]);
		//wxLogDebug("Bitmap: " + wxString::Format(wxT("%i"), currentBitmap) + " angularA: " + wxString::Format(wxT("%f"), angularA));
		return true;
	}
	
	return false;
	*/
}