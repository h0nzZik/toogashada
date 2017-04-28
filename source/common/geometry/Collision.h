#pragma once

#include <common/geometry/CircleShape.h>
#include <common/geometry/PolygonalShape.h>

namespace geometry {

bool collision(Point const &p1, CircleShape const &c1, Point const &p2, CircleShape const &c2);

}
