#pragma once

#include <wx/app.h>
#include "BoardGraph.h"

class Frame;

class Application : public wxApp
{
public:
	Application();
	virtual ~Application();

	virtual bool OnInit() override;
	virtual int OnExit() override;

	Frame* GetFrame();
	Clue::BoardGraph* GetBoardGraph();

private:
	Frame* frame;
	Clue::BoardGraph boardGraph;
};

wxDECLARE_APP(Application);