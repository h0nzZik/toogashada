#pragma once

template < typename T >
struct Point {
	T x;
	T y;
};

template < typename T >
struct Vector {
	T x;
	T y;
};

template < typename T >
Point<T> & operator+=(Point<T> & p, Vector<T> v) {
	p.x += v.x;
	p.y += v.y;
	return p;
}


template < typename T >
Point<T> operator+(Point<T> const & p, Vector<T> const &v) {
	Point<T> q = p;
	q += v;
	return q;
}

template < typename T >
Vector<T> operator+(Vector<T> const & v1, Vector<T> const &v2) {
	return Point<T>{v1.x + v2.x, v1.y + v2.y};
}

template < typename T >
Vector<T> operator-(Point<T> const & p1, Point<T> const &p2) {
	return Point<T>{p1.x + p2.x, p1.y + p2.y};
}



using IntPoint = Point<int>;
using IntVector = Vector<int>;

