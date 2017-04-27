#pragma once

#include <common/GPoint.h>
#include <stdint.h>
#include <vector>

#include "Tag.h"
#include "Message.h"
#include "Angle.h"
#include "EntityComponentSystem.h"

#include "common/components/EntityID.h"

// Server to client
// =====================================

struct MsgNewPolygonalObject {
	uint32_t object_id;
	Point center;
	std::vector<Point> shape;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(object_id, center, shape);
	}
};

struct MsgObjectPosition {
	uint32_t object_id;
	Point new_center;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(object_id, new_center);
	}
};

struct MsgNewPlayer {
	uint32_t player_id;
	uint32_t object_id;
	std::string playerName;
    std::string playerTeam;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(player_id, object_id, playerName, playerTeam);
	}
};

struct MsgNewEntity {
	EntityID entity_id;
	std::vector<AnyComponent> components;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(entity_id, components);
	}
};

struct MsgUpdateEntity {
	EntityID entity_id;
	std::vector<AnyComponent> components;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(entity_id, components);
	}
};

// Client to server
// =====================================

struct MsgIntroduceMyPlayer {

	std::string playerName;
	std::string playerTeam;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(playerName, playerTeam);
	}
};

struct MsgPlayerMovesLeft{};
struct MsgPlayerMovesRight{};
struct MsgPlayerMovesBackward{};
struct MsgPlayerMovesForward{};
struct MsgPlayerStops{};
