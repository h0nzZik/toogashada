#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "EntityComponentSystem.h"

#include "common/components/EntityID.h"

#include "PlayerAction.h"
#include "common/components/PlayerInfo.h"

#include "common/GameInfo.h"

// Server to client
// =====================================

// TODO: this will not be needed.
// We will send it as a new entity
struct MsgNewPlayer {
    uint32_t player_id;
    uint32_t object_id;
    std::string playerName;
    std::string playerTeam;

    template<typename Archive>
    void serialize(Archive &archive) {
        archive(player_id, object_id, playerName, playerTeam);
    }
};

struct MsgGameInfo {

    GameInfo gameInfo;

    template<typename Archive>
    void serialize(Archive &archive) {
        archive(gameInfo);
    }
};

struct MsgPlayerAssignedEntityId {

    EntityID entityId;

    template<typename Archive>
    void serialize(Archive &archive) {
        archive(entityId);
    }
};

struct MsgNewEntity {
    EntityID entity_id;
    std::vector<AnyComponent> components;

    template<typename Archive>
    void serialize(Archive &archive) {
        archive(entity_id, components);
    }
};

struct MsgUpdateComponents {
    EntityID entity_id;
    std::vector<AnyComponent> components;

    template<typename Archive>
    void serialize(Archive &archive) {
        archive(entity_id, components);
    }
};

struct MsgRemoveComponents {
	EntityID entity_id;
	std::vector<uint32_t> components;

    template<typename Archive>
    void serialize(Archive &archive) {
        archive(entity_id, components);
    }
};

struct MsgDeleteEntity {
    EntityID entity_id;

    template<typename Archive>
    void serialize(Archive &archive) {
        archive(entity_id);
    }
};

// Client to server
// =====================================

struct MsgIntroduceMyPlayer {

    PlayerInfo playerInfo;

    template<typename Archive>
    void serialize(Archive &archive) {
        archive(playerInfo);
    }
};

struct MsgPlayerActionChange {

    PlayerAction movement;
    int state;

    template<typename Archive>
    void serialize(Archive &archive) {
        archive(movement, state);
    }
};

struct MsgPlayerRotation {

    geometry::Angle rotation;

    template<typename Archive>
    void serialize(Archive &archive) {
        archive(rotation);
    }
};

// TODO not used for now, we update health based on entity component PLayerInfo
struct MsgPlayerHealth {

    int health;

    template<typename Archive>
    void serialize(Archive &archive) {
        archive(health);
    }
};
