#pragma once

#include <common/geometry/Angle.h>
#include <common/geometry/Point.h>
#include <common/geometry/Vector.h>

struct Position {
	geometry::Point center = {0,0};
	geometry::Vector speed = {0,0};
	geometry::Angle rotation = 0;
	geometry::AngularSpeed angularSpeed = 0; // We will probably not use this

	static Position create(geometry::Point point, geometry::Angle rotation = 0) {
		Position p;
		p.center = point;
		p.rotation = rotation;
		return p;
	}

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(center, speed, rotation, angularSpeed);
	}
};
