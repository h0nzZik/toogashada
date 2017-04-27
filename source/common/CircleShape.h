#pragma once

#include "GPoint.h"

template < typename T >
struct GCircleShape {
	T radius;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(radius);
	}
};

using CircleShape = GCircleShape<Scalar>;
