#include <common/Geometry.h>

#include <common/geometry/Polygon.h>

namespace geometry {

	Polygon makePolygon(Point const & center, PolygonalShape const & shape) {
		Polygon p;

		// TODO rotation
		for(auto const & v : shape)
			p.push_back(center + v);

		return p;
	}
}
