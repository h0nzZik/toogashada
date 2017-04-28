#ifndef COMMON_COMPONENTS_SHAPE_H_
#define COMMON_COMPONENTS_SHAPE_H_

#include <boost/variant/variant.hpp>

#include <common/Geometry.h>

using Shape = boost::variant<PolygonalShape, CircleShape>;

#endif /* COMMON_COMPONENTS_SHAPE_H_ */
