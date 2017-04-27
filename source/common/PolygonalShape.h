#pragma once

// Standard
#include <common/GPoint.h>
#include <vector>
#include <ostream>

// Common

template < typename T >
using GPolygonalShape = std::vector<GPoint<T>>;

template < typename T >
GPolygonalShape<T> & operator+=(GPolygonalShape<T> & shape, GVector<T> const &vec) {
	for(GPoint<T> & p : shape)
		p += vec;
	return *shape;
}

template < typename T >
std::ostream & operator<<(std::ostream &o, GPolygonalShape<T> const &shape ) {
	return o;
}

using PolygonalShape = GPolygonalShape<Scalar>;
