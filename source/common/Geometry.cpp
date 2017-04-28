#include <ostream>
#include <cmath>

#include "Geometry.h"

namespace geometry {

Scalar distance(Point const &p1, Point const &p2) {
	auto d = p2 - p1;
	return Scalar(std::hypot(d.x, d.y));
}

std::ostream & operator<<(std::ostream &o, Point const &point ) {
	o << "[" << point.x << "," << point.y << "]";
	return o;
}

std::ostream & operator<<(std::ostream &o, Vector const &point ) {
	o << "(" << point.x << "," << point.y << ")";
	return o;
}

std::ostream & operator<<(std::ostream &o, CircleShape const &shape ) {
	o << "{ radius: " << shape.radius << " }";
	return o;
}

PolygonalShape & operator+=(PolygonalShape & shape, Vector const &vec) {
	for(Point & p : shape)
		p += vec;
	return shape;
}

PolygonalShape & operator-=(PolygonalShape & shape, Vector const &vec) {
	for(Point & p : shape)
		p -= vec;
	return shape;
}

std::ostream & operator<<(std::ostream &o, PolygonalShape const &shape ) {
	o << "[";
	for(Point const & point : shape)
		o << " " << point;
	o << " ]";
	return o;
}

}
