#pragma once

#include <stdint.h>
#include "Point.h"

class GameObject {
	public:
		explicit GameObject(uint32_t object_id) : _object_id(object_id) {}
		virtual ~GameObject() = default;

		using Angle = int; // -180 to +179

		uint32_t object_id() const { return _object_id; }

		IntPoint center;
		std::vector<IntPoint> shape;
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
