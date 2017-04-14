#pragma once

// Standard
#include <vector>

// Common
#include <common/Point.h>

template < typename T >
using PolygonalShape = std::vector<Point<T>>;

template < typename T >
PolygonalShape<T> & operator+=(PolygonalShape<T> & shape, Vector<T> const &vec) {
	for(Point<T> & p : shape)
		p += vec;
	return *shape;
}

using IntPolygonalShape = PolygonalShape<int>;
