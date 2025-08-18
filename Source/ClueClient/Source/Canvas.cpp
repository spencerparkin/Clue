#include "Canvas.h"
#include "Application.h"
#include "Frame.h"
#include <gl/GLU.h>
#include <math.h>

using namespace Clue;

static int attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

Canvas::Canvas(wxWindow* parent) : wxGLCanvas(parent, wxID_ANY, attributeList, wxDefaultPosition, wxDefaultSize)
{
	this->renderContext = new wxGLContext(this);

	this->Bind(wxEVT_PAINT, &Canvas::OnPaint, this);
	this->Bind(wxEVT_SIZE, &Canvas::OnSize, this);
	this->Bind(wxEVT_MOTION, &Canvas::OnMouseMove, this);
	this->Bind(wxEVT_LEFT_DOWN, &Canvas::OnMouseClick, this);
}

/*virtual*/ Canvas::~Canvas()
{
	delete this->renderContext;
}

void Canvas::OnPaint(wxPaintEvent& event)
{
	this->SetCurrent(*this->renderContext);

	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	BoardGraph* boardGraph = wxGetApp().GetBoardGraph();
	if (boardGraph)
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		double aspectRatio = float(viewport[2]) / float(viewport[3]);

		this->worldBox = boardGraph->GetBoundingBox();
		this->worldBox.AddMargin(1.0);
		this->worldBox.MinimallyExpandToMatchAspectRatio(aspectRatio);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(this->worldBox.minCorner.x, this->worldBox.maxCorner.x, this->worldBox.minCorner.y, this->worldBox.maxCorner.y);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glBegin(GL_QUADS);

		boardGraph->ForAllNodes([this](const BoardGraph::Node* node) -> bool
			{
				this->RenderBoardNodeQuad(node);
				return true;
			});

		glColor3f(0.0f, 0.0f, 0.0f);

		boardGraph->ForAllNodes([this](const BoardGraph::Node* node) -> bool
			{
				this->RenderBoardNodeBorder(node);
				return true;
			});

		glEnd();
	}

	glFlush();

	this->SwapBuffers();
}

void Canvas::OnSize(wxSizeEvent& event)
{
	this->SetCurrent(*this->renderContext);

	wxSize size = event.GetSize();
	glViewport(0, 0, size.GetWidth(), size.GetHeight());

	this->Refresh();
}

void Canvas::RenderBoardNodeQuad(const BoardGraph::Node* node)
{
	const Vector2D& location = node->GetLocation();

	if (node->IsRoom())
		glColor3f(0.4f, 0.6f, 0.8f);
	else if (int(location.x + location.y) % 2 == 0)
		glColor3f(1.0f, 1.0f, 1.0f);
	else
		glColor3f(0.5f, 0.5f, 0.5f);

	std::vector<Vector2D> vertexArray;
	node->GetBox().GetVertices(vertexArray);
	for (const Vector2D& vertex : vertexArray)
		glVertex2d(vertex.x, vertex.y);
}

void Canvas::RenderBoardNodeBorder(const BoardGraph::Node* node)
{
	std::vector<const BoardGraph::Node*> adjacentNodeArray;
	node->GetAdjacencies(adjacentNodeArray);

	for (auto* adjacentNode : adjacentNodeArray)
	{
		if (node->IsPathway(adjacentNode))
			continue;

		Vector2D point = (node->GetLocation() + adjacentNode->GetLocation()) / 2.0;
		Vector2D minorAxis = point - node->GetLocation();
		Vector2D majorAxis = minorAxis.RotatedCCW90();
		Vector2D endPointA = point + majorAxis;
		Vector2D endPointB = point - majorAxis;
		Vector2D thicknessVector = minorAxis.Normalized() * 0.05;
		Vector2D vertexA = endPointA - thicknessVector;
		Vector2D vertexB = endPointB - thicknessVector;
		Vector2D vertexC = endPointB + thicknessVector;
		Vector2D vertexD = endPointA + thicknessVector;

		glVertex2d(vertexA.x, vertexA.y);
		glVertex2d(vertexB.x, vertexB.y);
		glVertex2d(vertexC.x, vertexC.y);
		glVertex2d(vertexD.x, vertexD.y);
	}
}

Vector2D Canvas::MousePointToWorldPoint(const wxPoint& mousePoint)
{
	Box2D canvasBox;
	canvasBox.minCorner = Vector2D(0.0, 0.0);
	canvasBox.maxCorner = Vector2D(this->GetSize().x, this->GetSize().y);

	Vector2D canvasPoint(mousePoint.x, this->GetSize().y - 1 - mousePoint.y);
	Vector2D uv;
	canvasBox.PointToUVs(canvasPoint, uv);

	Vector2D worldPoint;
	this->worldBox.PointFromUVs(worldPoint, uv);
	return worldPoint;
}

void Canvas::RenderCircle(const Vector2D& center, double radius, double r, double g, double b, int numSegments /*= 12*/)
{
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.0, 0.0, 0.0);
	glVertex2d(center.x, center.y);

	for (int i = 0; i <= numSegments; i++)
	{
		double angle = 2.0 * M_PI * double(i) / double(numSegments);
		Vector2D vertex;
		vertex.Compose(radius, angle);
		vertex += center;
		glVertex2d(vertex.x, vertex.y);
	}

	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(r, g, b);
	glVertex2d(center.x, center.y);

	for (int i = 0; i <= numSegments; i++)
	{
		double angle = 2.0 * M_PI * double(i) / double(numSegments);
		Vector2D vertex;
		vertex.Compose(radius * 0.8, angle);
		vertex += center;
		glVertex2d(vertex.x, vertex.y);
	}

	glEnd();
}

void Canvas::OnMouseMove(wxMouseEvent& event)
{
}

void Canvas::OnMouseClick(wxMouseEvent& event)
{
}