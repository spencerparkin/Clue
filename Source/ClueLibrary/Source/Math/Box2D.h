#pragma once

#include "../Defines.h"
#include "Vector2D.h"
#include <vector>

namespace Clue
{
	/**
	 * These are axis-aligned boxes in the plane.
	 */
	class CLUE_LIBRARY_API Box2D
	{
	public:
		Box2D();
		Box2D(const Vector2D& minCorner, const Vector2D& maxCorner);
		Box2D(const Box2D& box);
		virtual ~Box2D();

		void operator=(const Box2D& box);

		double Width() const;
		double Height() const;
		double AspectRatio() const;
		double Area() const;
		Vector2D Center() const;
		void MinimallyExpandToMatchAspectRatio(double aspectRatio);
		void MinimallyContractToMatchAspectRatio(double aspectRatio);
		bool ContainsPoint(const Vector2D& point) const;
		void ExpandToIncludePoint(const Vector2D& point);
		void AddMargin(double margin);
		void PointToUVs(const Vector2D& point, Vector2D& uv) const;
		void PointFromUVs(Vector2D& point, const Vector2D& uv) const;
		void GetVertices(std::vector<Vector2D>& vertexArray) const;
		Box2D ScaledAboutCenter(double scale) const;

	public:
		Vector2D minCorner;
		Vector2D maxCorner;
	};
}