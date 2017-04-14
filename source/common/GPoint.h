#pragma once

/**
 * GPoint - a generic point
 */
template < typename T >
struct GPoint {
	T x;
	T y;
};

/**
 * GVector - a generic vector
 */
template < typename T >
struct GVector {
	T x;
	T y;
};

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



using Scalar = float;
using Point = GPoint<Scalar>;
using Vector = GVector<Scalar>;

