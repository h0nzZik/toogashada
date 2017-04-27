#pragma once

// Standard
#include <common/GPoint.h>
#include <vector>

// Common

template < typename T >
using GPolygonalShape = std::vector<GPoint<T>>;

template < typename T >
GPolygonalShape<T> & operator+=(GPolygonalShape<T> & shape, GVector<T> const &vec) {
	for(GPoint<T> & p : shape)
		p += vec;
	return *shape;
}

using PolygonalShape = GPolygonalShape<Scalar>;
