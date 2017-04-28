#pragma once

#include <cassert>
#include <common/geometry/Point.h>

namespace geometry {

class RectangularArea {
	Point topLeft_;
	Point bottomRight_;

public:
	RectangularArea() {
		topLeft_ = {0,0};
		bottomRight_ = {0,0};
	}

	RectangularArea(Point topLeft, Point bottomRight) :
		topLeft_(topLeft), bottomRight_(bottomRight) {
		assert(topLeft.x <= bottomRight.x);
		assert(topLeft.y <= bottomRight.y);
	}

	RectangularArea(RectangularArea const & orig) = default;
	RectangularArea & operator=(RectangularArea const & orig) = default;

	Point const & topLeft() const { return topLeft_; }
	Point const & bottomRight() const { return bottomRight_; }
};

} // namespace geometry
