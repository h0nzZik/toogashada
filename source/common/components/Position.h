#pragma once

#include <common/Angle.h>
#include <common/Geometry.h>

struct Position {
	Point center;
	Vector speed;
	Angle rotation;
	Angle angularSpeed;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(center, speed, rotation, angularSpeed);
	}
};
