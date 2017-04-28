#pragma once

#include <common/geometry/Point.h>
#include <common/geometry/Vector.h>
#include <common/geometry/PolygonalShape.h>
#include <common/geometry/CircleShape.h>
#include <common/geometry/Circle.h>
#include <common/geometry/RectangularArea.h>

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

Scalar cos(Vector const &a, Vector const &b);

// Size of vector
Scalar size(Vector const &vec);

// Dot product
Scalar dot(Vector const &v1, Vector const &v2);

Vector project(Vector const &v1, Vector const &v2);

inline Vector & operator+=(Vector & v1, Vector const &v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	return v1;
}

inline Vector & operator-=(Vector & v1, Vector const &v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	return v1;
}

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

inline Vector operator/(Vector const & vector, Scalar scalar) {
	return vector * (1.0/scalar);
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
std::ostream & operator<<(std::ostream &o, RectangularArea const &area );

bool in(Point const & point, RectangularArea const &area);
bool in(Circle const & circle, RectangularArea const &area);
bool in(Point const & point, CircleShape const & shape, RectangularArea const &area);
bool in(Point const & point, PolygonalShape const & shape, RectangularArea const &area);

} // namespace geometry
