#ifndef COMMON_COMPONENTS_SHAPE_H_
#define COMMON_COMPONENTS_SHAPE_H_

#include <boost/variant/variant.hpp>

#include <common/geometry/CircleShape.h>
#include <common/geometry/PolygonalShape.h>

using Shape = boost::variant<geometry::PolygonalShape, geometry::CircleShape>;

#endif /* COMMON_COMPONENTS_SHAPE_H_ */
