#pragma once

#include <common/geometry/CircleShape.h>

namespace geometry {

class Circle {
public:
	Point center;
	Scalar radius;

	Circle() :
		center{}, radius{} {
	}

	Circle(Point center, Scalar radius) :
		center{center}, radius{radius} {
	}

	Circle(Point center, CircleShape shape) :
		Circle(center, shape.radius) {
	}

	Circle(Circle const & orig) = default;
	Circle & operator=(Circle const & orig) = default;

	bool operator==(Circle const &other) const {
		return center == other.center
			&& radius == other.radius;
	}

	bool operator!=(Circle const &other) const {
		return !(*this == other);
	}
};

} // namespace geometry
