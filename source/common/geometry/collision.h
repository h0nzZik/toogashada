#pragma once

#include <common/geometry/Point.h>
#include <common/geometry/Polygon.h>

namespace geometry {

class Circle;

bool collision(Circle const &c1, Circle const &c2);
bool collision(Polygon const &p1, Polygon const &p2);
bool collision(Polygon const &polygon, Circle const &circle);
bool collision(Circle const &circle, Polygon const &polygon);

struct MinMax {
	Scalar min;
	Scalar max;
};

MinMax minmaxProjection(Polygon const &polygon, Vector axis);
bool gapAlongAxis(Vector axis, Polygon const &p1, Polygon const &p2);

} // namespace geometry
