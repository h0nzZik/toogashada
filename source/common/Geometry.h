#pragma once

#include <vector>
#include "Scalar.h"

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


struct Point {
	Scalar x;
	Scalar y;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(x,y);
	}
};

struct Vector {
	Scalar x;
	Scalar y;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(x,y);
	}
};

inline bool operator==(Vector const &l, Vector const &r) {
	return (l.x == r.x) && (l.y == r.y);
}

inline bool operator!=(Vector const &l, Vector const &r) {
	return !(l == r);
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

inline Point & operator+=(Point & p, Vector v) {
	p.x += v.x;
	p.y += v.y;
	return p;
}

inline Point operator+(Point const & p, Vector const &v) {
	Point q = p;
	q += v;
	return q;
}

inline Vector operator+(Vector const & v1, Vector const &v2) {
	return Vector{v1.x + v2.x, v1.y + v2.y};
}

inline Vector operator-(Point const & p1, Point const &p2) {
	return Vector{p1.x + p2.x, p1.y + p2.y};
}

std::ostream & operator<<(std::ostream &o, Point const &point );
std::ostream & operator<<(std::ostream &o, Vector const &point );

struct CircleShape {
	Scalar radius;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(radius);
	}
};

std::ostream & operator<<(std::ostream &o, CircleShape const &shape );

using PolygonalShape = std::vector<Point>;

PolygonalShape & operator+=(PolygonalShape & shape, Vector const &vec);

std::ostream & operator<<(std::ostream &o, PolygonalShape const &shape );
