#pragma once

#include <stdint.h>
#include <vector>

#include "Tag.h"
#include "Message.h"
#include "Point.h"

struct MsgNewPolygonalObject {
	static constexpr Tag tag = Tag::NewPolygonalObject;
	
	uint32_t object_id;
	std::vector<IntPoint> points;

	Message to_message() const;

	static MsgNewPolygonalObject from(Message msg);
};
