#pragma once

#include <common/geometry/Point.h>
#include <common/geometry/Vector.h>
#include <common/geometry/PolygonalShape.h>
#include <common/geometry/CircleShape.h>


// For streaming operators
namespace std
{
template <class charT>
struct char_traits;
template <>
struct char_traits<char>;
template <class charT, class traits>
class basic_ostream;
typedef basic_ostream<char, char_traits<char> > ostream;
}

namespace geometry {


Scalar distance(Point const &p1, Point const &p2);

inline Vector & operator*=(Vector & vector, Scalar scalar) {
	vector.x *= scalar;
	vector.y *= scalar;
	return vector;
}

inline Vector operator*(Vector const & vector, Scalar scalar) {
	Vector vec = vector;
	vec *= scalar;
	return vec;
}

inline Vector operator*(Scalar scalar, Vector const & vector) {
	return vector * scalar;
}

inline Point & operator+=(Point & p, Vector v) {
	p.x += v.x;
	p.y += v.y;
	return p;
}

inline Point & operator-=(Point & p, Vector v) {
	p.x -= v.x;
	p.y -= v.y;
	return p;
}

inline Point operator+(Point const & p, Vector const &v) {
	Point q = p;
	q += v;
	return q;
}

inline Vector operator+(Vector const &v) {
	return {+v.x, +v.y};
}

inline Vector operator-(Vector const &v) {
	return {-v.x, -v.y};
}

inline Vector operator+(Vector const & v1, Vector const &v2) {
	return Vector{v1.x + v2.x, v1.y + v2.y};
}

inline Vector operator-(Point const & p1, Point const &p2) {
	return Vector{p1.x - p2.x, p1.y - p2.y};
}

PolygonalShape & operator+=(PolygonalShape & shape, Vector const &vec);

std::ostream & operator<<(std::ostream &o, Point const &point );
std::ostream & operator<<(std::ostream &o, Vector const &point );
std::ostream & operator<<(std::ostream &o, CircleShape const &shape );
std::ostream & operator<<(std::ostream &o, PolygonalShape const &shape );

} // namespace geometry
