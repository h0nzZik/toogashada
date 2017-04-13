#pragma once

#include <stdint.h>
#include <vector>

#include "Tag.h"
#include "Message.h"
#include "Point.h"
#include "Angle.h"

struct MsgNewPolygonalObject {
	static constexpr Tag tag = Tag::NewPolygonalObject;
	
	uint32_t object_id;
	IntPoint center;
	std::vector<IntPoint> shape;

	Message to_message() const;

	static MsgNewPolygonalObject from(Message const &msg);
};

struct MsgNewObjectPosition {
	uint32_t object_id;
	IntPoint new_center;
};

struct MsgNewPlayer {
	uint32_t player_id;
	uint32_t object_id;
	std::string player_name;

	static constexpr Tag tag = Tag::NewPlayer;
	Message to_message() const;
	static MsgNewPlayer from(Message const & msg);
};

struct MsgSetPlayerMovement {
	IntPoint speed;
	Angle angle;
	AngularSpeed angularSpeed;

	static constexpr Tag tag = Tag::NewPlayer;
	Message to_message() const;
	static MsgSetPlayerMovement from(Message const & msg);
};
