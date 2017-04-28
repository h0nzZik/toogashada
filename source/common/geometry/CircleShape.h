#pragma once

#include <common/Scalar.h>

namespace geometry {

class CircleShape {
public:
	Scalar radius;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(radius);
	}
};

} // namespace geometry
