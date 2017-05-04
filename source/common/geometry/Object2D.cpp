#include <boost/variant/static_visitor.hpp>
#include <common/Geometry.h>
#include <common/geometry/Object2D.h>

namespace geometry {

Polygon createPolygon(Point const &center, Angle const &angle, PolygonalShape const &shape) {
	Polygon polygon;
	// TODO do some rotation on shape
	for(Vector const & vec : shape)
		polygon.push_back(center + rotate(vec, angle));

	return polygon;
}

Object2D createObject2D(Point const &point, Angle const & angle, Shape const & shape) {

	class CreateObject2D : public boost::static_visitor<Object2D>{
	private:
		Point const &point;
		Angle const &angle;
	public:
		CreateObject2D(Point const &point, Angle const &angle) :
			point(point), angle(angle) {
		}

		Object2D operator()(CircleShape const & shape) {
			return {Circle{point, shape}};
		}

		Object2D operator()(PolygonalShape const &shape) {
			return createPolygon(point, angle, shape);
		}
	};

	CreateObject2D co2d{point, angle};
	return boost::apply_visitor(co2d, shape);
}

} // namespace geometry
