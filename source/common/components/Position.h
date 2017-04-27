#pragma once

#include <common/GPoint.h>
#include <common/Angle.h>

template < typename T >
struct GPosition {
	GPoint<T> center;
	GVector<T> speed;
	Angle rotation;
	Angle angularSpeed;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(center, speed, rotation, angularSpeed);
	}
};

using Position = GPosition<Scalar>;
