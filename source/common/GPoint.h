#pragma once

#include <ostream>

/**
 * GPoint - a generic point
 */
template < typename T >
struct GPoint {
	T x;
	T y;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(x,y);
	}
};

/**
 * GVector - a generic vector
 */
template < typename T >
struct GVector {
	T x;
	T y;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(x,y);
	}
};

template < typename T >
bool operator==(GVector<T> const &l, GVector<T> const &r) {
	return (l.x == r.x) && (l.y == r.y);
}

template < typename T >
bool operator!=(GVector<T> const &l, GVector<T> const &r) {
	return !(l == r);
}

template < typename T >
GVector<T> & operator*=(GVector<T> & vector, T scalar) {
	vector.x *= scalar;
	vector.y *= scalar;
	return vector;
}

template < typename T >
GVector<T> operator*(GVector<T> const & vector, T scalar) {
	GVector<T> vec = vector;
	vec *= scalar;
	return vec;
}

template < typename T >
GVector<T> operator*(T scalar, GVector<T> const & vector) {
	return vector * scalar;
}


template < typename T >
GPoint<T> & operator+=(GPoint<T> & p, GVector<T> v) {
	p.x += v.x;
	p.y += v.y;
	return p;
}


template < typename T >
GPoint<T> operator+(GPoint<T> const & p, GVector<T> const &v) {
	GPoint<T> q = p;
	q += v;
	return q;
}

template < typename T >
GVector<T> operator+(GVector<T> const & v1, GVector<T> const &v2) {
	return GPoint<T>{v1.x + v2.x, v1.y + v2.y};
}

template < typename T >
GVector<T> operator-(GPoint<T> const & p1, GPoint<T> const &p2) {
	return GPoint<T>{p1.x + p2.x, p1.y + p2.y};
}

template < typename T >
std::ostream & operator<<(std::ostream &o, GPoint<T> const &point ) {
	o << "[" << point.x << "," << point.y << "]";
	return o;
}

template < typename T >
std::ostream & operator<<(std::ostream &o, GVector<T> const &point ) {
	o << "(" << point.x << "," << point.y << ")";
	return o;
}


using Scalar = float;
using Point = GPoint<Scalar>;
using Vector = GVector<Scalar>;

