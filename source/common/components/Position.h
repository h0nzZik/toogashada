#pragma once

#include <common/geometry/Angle.h>
#include <common/geometry/Point.h>
#include <common/geometry/Vector.h>

struct Position {
	geometry::Point center;
	geometry::Vector speed;
	geometry::Angle rotation;
	geometry::AngularSpeed angularSpeed;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(center, speed, rotation, angularSpeed);
	}
};
