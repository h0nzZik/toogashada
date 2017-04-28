#pragma once

#include <ostream>
#include "GPoint.h"

template < typename T >
struct GCircleShape {
	T radius;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(radius);
	}
};


template < typename T >
std::ostream & operator<<(std::ostream &o, GCircleShape<T> const &shape ) {
	o << "{ radius: " << shape.radius << " }";
}


using CircleShape = GCircleShape<Scalar>;
