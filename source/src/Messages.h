#pragma once

#include <stdint.h>
#include <vector>

#include "Tag.h"
#include "Message.h"
#include "Point.h"

struct MsgNewPolygonalObject {
	static constexpr Tag tag = Tag::NewPolygonalObject;
	
	uint32_t object_id;
	IntPoint center;
	std::vector<IntPoint> shape;

	Message to_message() const;

	static MsgNewPolygonalObject from(Message msg);
};

struct MsgNewObjectPosition {
	uint32_t object_id;
	IntPoint new_center;
};
