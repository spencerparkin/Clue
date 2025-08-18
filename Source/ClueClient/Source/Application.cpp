#include "Application.h"
#include "Frame.h"

wxIMPLEMENT_APP(Application);

Application::Application()
{
	this->frame = nullptr;
}

/*virtual*/ Application::~Application()
{
}

/*virtual*/ bool Application::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	this->boardGraph.Regenerate();

	//this->frame = new Frame(wxDefaultPosition, wxSize(1200, 800));
	this->frame = new Frame(wxDefaultPosition, wxSize(2400, 1600));
	this->frame->Show();

	return true;
}

/*virtual*/ int Application::OnExit()
{
	return 0;
}

Frame* Application::GetFrame()
{
	return this->frame;
}

Clue::BoardGraph* Application::GetBoardGraph()
{
	return &this->boardGraph;
}