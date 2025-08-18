#pragma once

#include <wx/glcanvas.h>
#include "Math/Box2D.h"
#include "Math/Vector2D.h"
#include "BoardGraph.h"

class Canvas : public wxGLCanvas
{
public:
	Canvas(wxWindow* parent);
	virtual ~Canvas();

	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseClick(wxMouseEvent& event);

private:

	void RenderBoardNodeQuad(const Clue::BoardGraph::Node* node);
	void RenderBoardNodeBorder(const Clue::BoardGraph::Node* node);
	void RenderCircle(const Clue::Vector2D& center, double radius, double r, double g, double b, int numSegments = 12);
	Clue::Vector2D MousePointToWorldPoint(const wxPoint& mousePoint);

	wxGLContext* renderContext;
	Clue::Box2D worldBox;
};