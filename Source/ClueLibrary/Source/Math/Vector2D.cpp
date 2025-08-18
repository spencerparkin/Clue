#include "Vector2D.h"
#include <math.h>

using namespace Clue;

Vector2D::Vector2D()
{
	this->x = 0.0;
	this->y = 0.0;
}

Vector2D::Vector2D(double x, double y)
{
	this->x = x;
	this->y = y;
}

Vector2D::Vector2D(const Vector2D& vector)
{
	this->x = vector.x;
	this->y = vector.y;
}

/*virtual*/ Vector2D::~Vector2D()
{
}

void Vector2D::operator=(const Vector2D& vector)
{
	this->x = vector.x;
	this->y = vector.y;
}

void Vector2D::operator+=(const Vector2D& vector)
{
	this->x += vector.x;
	this->y += vector.y;
}

void Vector2D::operator-=(const Vector2D& vector)
{
	this->x -= vector.x;
	this->y -= vector.y;
}

void Vector2D::operator*=(double scalar)
{
	this->x *= scalar;
	this->y *= scalar;
}

void Vector2D::operator/=(double scalar)
{
	this->x /= scalar;
	this->y /= scalar;
}

Vector2D Vector2D::operator*(double scalar) const
{
	return Vector2D(this->x * scalar, this->y * scalar);
}

Vector2D Vector2D::operator/(double scalar) const
{
	return Vector2D(this->x / scalar, this->y / scalar);
}

double Vector2D::Dot(const Vector2D& vector) const
{
	return this->x * vector.x + this->y * vector.y;
}

double Vector2D::Cross(const Vector2D& vector) const
{
	return this->x * vector.y - this->y * vector.x;
}

double Vector2D::Length() const
{
	return ::sqrt(this->x * this->x + this->y * this->y);
}

Vector2D Vector2D::Normalized() const
{
	return *this / this->Length();
}

Vector2D Vector2D::ProjectedOnto(const Vector2D& unitVector) const
{
	return unitVector * this->Dot(unitVector);
}

Vector2D Vector2D::RejectedFrom(const Vector2D& unitVector) const
{
	return *this - this->ProjectedOnto(unitVector);
}

Vector2D Vector2D::RotatedBy(double rotationAngle) const
{
	double radius = 0.0;
	double angle = 0.0;

	this->Decompose(radius, angle);

	angle += rotationAngle;

	Vector2D rotatedVector;
	rotatedVector.Decompose(radius, angle);
	return rotatedVector;
}

Vector2D Vector2D::RotatedCCW90() const
{
	return Vector2D(-this->y, this->x);
}

void Vector2D::Decompose(double& radius, double& angle) const
{
	radius = this->Length();
	angle = ::atan2(this->y, this->x);
}

void Vector2D::Compose(double radius, double angle)
{
	this->x = radius * ::cos(angle);
	this->y = radius * ::sin(angle);
}

namespace Clue
{
	Vector2D operator+(const Vector2D& vectorA, const Vector2D& vectorB)
	{
		return Vector2D(vectorA.x + vectorB.x, vectorA.y + vectorB.y);
	}

	Vector2D operator-(const Vector2D& vectorA, const Vector2D& vectorB)
	{
		return Vector2D(vectorA.x - vectorB.x, vectorA.y - vectorB.y);
	}

	bool operator==(const Vector2D& vectorA, const Vector2D& vectorB)
	{
		return vectorA.x == vectorB.x && vectorA.y == vectorB.y;
	}
}