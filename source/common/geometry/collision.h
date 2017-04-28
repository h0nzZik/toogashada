#pragma once

#include <common/geometry/Point.h>
#include <common/geometry/Polygon.h>
#include <common/geometry/Object2D.h>


namespace geometry {

class Circle;

bool collision(Circle const &c1, Circle const &c2);
bool collision(Polygon const &p1, Polygon const &p2);
bool collision(Polygon const &polygon, Circle const &circle);
bool collision(Circle const &circle, Polygon const &polygon);
bool collision(Polygon const &polygon, Object2D const &object);
bool collision(Circle const &circle, Object2D const &object);
bool collision(Object2D const &object1, Object2D const &object2);

struct MinMax {
	Scalar min;
	Scalar max;
};

MinMax minmaxProjection(Polygon const &polygon, Vector axis);
bool gapAlongAxis(Vector axis, Polygon const &p1, Polygon const &p2);

} // namespace geometry
