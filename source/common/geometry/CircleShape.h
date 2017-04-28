#pragma once

#include <common/Scalar.h>

namespace geometry {

struct CircleShape {
	Scalar radius;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(radius);
	}
};

} // namespace geometry
