#pragma once

#include <stdint.h>
#include "Point.h"
#include "Angle.h"

class GameObject {
	public:
		explicit GameObject(uint32_t object_id) : _object_id(object_id) {}
		virtual ~GameObject() = default;

		uint32_t id() const { return _object_id; }

		IntPoint center;
		std::vector<IntPoint> shape; // this should be immutable
#if 0
	protected:
		IntPoint _center;
		IntPoint _speed;
		Angle _angle;
		Angle _angular_speed;
#endif
	private:
		uint32_t _object_id;
};
