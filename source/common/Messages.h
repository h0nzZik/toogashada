#pragma once

#include <stdint.h>
#include <vector>
#include <string>

#include "EntityComponentSystem.h"

#include "common/components/EntityID.h"

#include "PlayerAction.h"

// Server to client
// =====================================

// TODO: this will not be needed.
// We will send it as a new entity
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

struct MsgGameInfo {
	Scalar area_size_x;
	Scalar area_size_y;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(area_size_x, area_size_y);
	}
};

struct MsgPlayerAssignedEntityId {

	EntityID entityId;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(entityId);
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

struct MsgDeleteEntity {
	EntityID entity_id;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(entity_id);
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
struct MsgPlayerShoots{};

struct MsgPlayerActionChange{

    PlayerAction movement;
    int state;

    template < typename Archive >
    void serialize(Archive & archive) {
        archive(movement, state);
    }
};

struct MsgPlayerRotation {

	double absoluteRotation; // value is player's heading (north is on top)

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(absoluteRotation);
	}
};

struct MsgPlayerHealth {

	int health;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(health);
	}
};
