#include <boost/variant/static_visitor.hpp>

#include <common/Geometry.h>
#include <common/components/Shape.h>

using namespace geometry;

bool in(Point const & point, Shape const & shape, RectangularArea const &area) {
	class Visitor : public boost::static_visitor<bool> {
		Point const & point;
		RectangularArea const &area;
	public:
		Visitor(Point const & point, RectangularArea const & area) :
			point(point), area(area) { ; }

		bool operator()(CircleShape const & shape) {
			return geometry::in(point, shape, area);
		}

		bool operator()(PolygonalShape const & shape) {
			return geometry::in(point, shape, area);
		}
	};
	Visitor visitor{point, area};
	return boost::apply_visitor(visitor, shape);
}

