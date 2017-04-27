#ifndef COMMON_COMPONENTS_SHAPE_H_
#define COMMON_COMPONENTS_SHAPE_H_

#include <boost/variant/variant.hpp>

#include <common/PolygonalShape.h>
#include <common/CircleShape.h>

template < typename T >
using GShape = boost::variant<GPolygonalShape<T>, GCircleShape<T>>;

using Shape = GShape<Scalar>;

#endif /* COMMON_COMPONENTS_SHAPE_H_ */
