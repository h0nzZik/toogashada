#pragma once

#include <vector>
#include <common/geometry/Point.h>
#include <common/geometry/PolygonalShape.h>

namespace geometry {
	using Polygon = std::vector<geometry::Point>;

	Polygon makePolygon(Point const & center, PolygonalShape const & shape);
}
