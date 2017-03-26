#pragma once

#include <stdint.h>
#include "Point.h"

class TwoDimensionalObject {
	public:
		explicit TwoDimensionalObject(uint32_t object_id) : _object_id(object_id) {}
		virtual ~TwoDimensionalObject() = default;

		using Angle = int; // -180 to +179

		uint32_t object_id() const { return _object_id; }

	protected:
		IntPoint _center;
		IntPoint _speed;
		Angle _angle;
		Angle _angular_speed;

	private:
		uint32_t _object_id;
};
