#include "Box2D.h"
#include <limits>

using namespace Clue;

Box2D::Box2D()
{
	this->minCorner.x = std::numeric_limits<double>::max();
	this->minCorner.y = std::numeric_limits<double>::max();
	this->maxCorner.x = std::numeric_limits<double>::min();
	this->maxCorner.y = std::numeric_limits<double>::min();
}

Box2D::Box2D(const Vector2D& minCorner, const Vector2D& maxCorner)
{
	this->minCorner = minCorner;
	this->maxCorner = maxCorner;
}

Box2D::Box2D(const Box2D& box)
{
	this->minCorner = box.minCorner;
	this->maxCorner = box.maxCorner;
}

/*virtual*/ Box2D::~Box2D()
{
}

void Box2D::operator=(const Box2D& box)
{
	this->minCorner = box.minCorner;
	this->maxCorner = box.maxCorner;
}

double Box2D::Width() const
{
	return this->maxCorner.x - this->minCorner.x;
}

double Box2D::Height() const
{
	return this->maxCorner.y - this->minCorner.y;
}

double Box2D::AspectRatio() const
{
	return this->Width() / this->Height();
}

double Box2D::Area() const
{
	return this->Width() * this->Height();
}

Vector2D Box2D::Center() const
{
	return (this->minCorner + this->maxCorner) / 2.0;
}

void Box2D::MinimallyExpandToMatchAspectRatio(double aspectRatio)
{
	double currentAspectRatio = this->AspectRatio();

	if (currentAspectRatio < aspectRatio)
	{
		double delta = (this->Height() * aspectRatio - this->Width()) / 2.0;
		this->minCorner.x -= delta;
		this->maxCorner.x += delta;
	}
	else if (currentAspectRatio > aspectRatio)
	{
		double delta = (this->Width() / aspectRatio - this->Height()) / 2.0;
		this->minCorner.y -= delta;
		this->maxCorner.y += delta;
	}
}

void Box2D::MinimallyContractToMatchAspectRatio(double aspectRatio)
{
	// TODO: Write this.
}

bool Box2D::ContainsPoint(const Vector2D& point) const
{
	if (point.x < this->minCorner.x || point.x > this->maxCorner.x)
		return false;

	if (point.y < this->minCorner.y || point.y > this->maxCorner.y)
		return false;

	return true;
}

void Box2D::ExpandToIncludePoint(const Vector2D& point)
{
	if (this->minCorner.x > point.x)
		this->minCorner.x = point.x;

	if (this->maxCorner.x < point.x)
		this->maxCorner.x = point.x;

	if (this->minCorner.y > point.y)
		this->minCorner.y = point.y;

	if (this->maxCorner.y < point.y)
		this->maxCorner.y = point.y;
}

void Box2D::AddMargin(double margin)
{
	this->minCorner.x -= margin;
	this->maxCorner.x += margin;
	this->minCorner.y -= margin;
	this->maxCorner.y += margin;
}

void Box2D::PointToUVs(const Vector2D& point, Vector2D& uv) const
{
	uv.x = (point.x - this->minCorner.x) / this->Width();
	uv.y = (point.y - this->minCorner.y) / this->Height();
}

void Box2D::PointFromUVs(Vector2D& point, const Vector2D& uv) const
{
	point.x = this->minCorner.x + uv.x * this->Width();
	point.y = this->minCorner.y + uv.y * this->Height();
}

void Box2D::GetVertices(std::vector<Vector2D>& vertexArray) const
{
	vertexArray.push_back(Vector2D(this->minCorner.x, this->minCorner.y));
	vertexArray.push_back(Vector2D(this->maxCorner.x, this->minCorner.y));
	vertexArray.push_back(Vector2D(this->maxCorner.x, this->maxCorner.y));
	vertexArray.push_back(Vector2D(this->minCorner.x, this->maxCorner.y));
}

Box2D Box2D::ScaledAboutCenter(double scale) const
{
	Vector2D center = this->Center();
	Vector2D delta = this->maxCorner - center;
	delta *= scale;
	Box2D scaledBox;
	scaledBox.minCorner = center - delta;
	scaledBox.maxCorner = center + delta;
	return scaledBox;
}