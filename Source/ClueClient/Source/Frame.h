#pragma once

#include <wx/frame.h>

class Canvas;

class Frame : public wxFrame
{
public:
	Frame(const wxPoint& pos, const wxSize& size);
	virtual ~Frame();

	enum
	{
		ID_Exit = wxID_HIGHEST,
		ID_About
	};

	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnUpdateUI(wxUpdateUIEvent& event);

	Canvas* GetCanvas();

private:

	Canvas* canvas;
};