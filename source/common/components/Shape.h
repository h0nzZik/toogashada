#pragma once

#include <boost/variant/variant.hpp>

#include <common/geometry/RectangularArea.h>
#include <common/geometry/CircleShape.h>
#include <common/geometry/PolygonalShape.h>

using Shape = boost::variant<geometry::PolygonalShape, geometry::CircleShape>;

//bool in(geometry::Point const & point, Shape const & shape, geometry::RectangularArea const &area);
