#pragma once

#include <boost/variant/variant.hpp>

#include <common/geometry/Angle.h>
#include <common/geometry/Circle.h>
#include <common/geometry/Polygon.h>

#include <common/components/Shape.h>

namespace geometry {

using Object2D = boost::variant<Polygon, Circle>;

Polygon createPolygon(Point const &center, Angle const &angle, PolygonalShape const &shape);
Object2D createObject2D(Point const &point, Angle const & angle, Shape const & shape);

} // namespace geometry
