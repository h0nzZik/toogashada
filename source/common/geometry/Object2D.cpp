#include <boost/variant/static_visitor.hpp>
#include <common/Geometry.h>
#include <common/geometry/Object2D.h>

namespace geometry {

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
			(void)angle;
			Polygon polygon;
			// TODO do some rotation on shape
			for(Vector const & vec : shape)
				polygon.push_back(point + vec);

			return {polygon};
		}
	};

	CreateObject2D co2d{point, angle};
	return boost::apply_visitor(co2d, shape);
}

} // namespace geometry
