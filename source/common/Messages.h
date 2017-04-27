#pragma once

#include <common/GPoint.h>
#include <stdint.h>
#include <vector>

#include "Tag.h"
#include "Message.h"
#include "Angle.h"

#include "common/components/EntityID.h"

struct MsgNewPolygonalObject {
	static constexpr Tag tag = Tag::NewPolygonalObject;
	
	uint32_t object_id;
	Point center;
	std::vector<Point> shape;

	Message to_message() const;

	static MsgNewPolygonalObject from(Message const &msg);
};

struct MsgObjectPosition {
	uint32_t object_id;
	Point new_center;

	static constexpr Tag tag = Tag::ObjectPosition;
	Message to_message() const;
	static MsgObjectPosition from(Message const &msg);
};

struct MsgNewPlayer {
	uint32_t player_id;
	uint32_t object_id;
	std::string playerName;
    std::string playerTeam;

	static constexpr Tag tag = Tag::NewPlayer;
	Message to_message() const;
	static MsgNewPlayer from(Message const & msg);
};

struct MsgSetPlayerMovement {
	Point speed;
	Angle angle;
	AngularSpeed angularSpeed;

	static constexpr Tag tag = Tag::NewPlayer;
	Message to_message() const;
	static MsgSetPlayerMovement from(Message const & msg);
};

struct MsgIntroduceMyPlayer {

	std::string playerName;
	std::string playerTeam;

    static constexpr Tag tag = Tag::IntroduceMyPlayer;
    Message to_message() const;
    static MsgIntroduceMyPlayer from(Message const & msg);
};

struct AnyComponent;

struct MsgNewEntity {
	EntityID entity_id;
	std::vector<AnyComponent> components;

	static constexpr Tag tag = Tag::NewEntity;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(entity_id, components);
	}

	Message to_message() const;
	static MsgNewEntity from(Message const & msg);
};


struct MsgUpdateEntity {
	EntityID entity_id;
	std::vector<AnyComponent> components;

	static constexpr Tag tag = Tag::UpdateEntity;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(entity_id, components);
	}

	Message to_message() const;
	static MsgUpdateEntity from(Message const & msg);
};

