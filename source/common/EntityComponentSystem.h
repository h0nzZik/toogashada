#pragma once

#include <common/BaseEntityComponentSystem.h>
#include <common/GameInfo.h>
#include <common/components/EntityID.h>
#include <common/components/Explosive.h>
#include <common/components/Health.h>
#include <common/components/Player.h>
#include <common/components/PlayerInfo.h>
#include <common/components/Position.h>
#include <common/components/Shape.h>
#include <common/geometry/Object2D.h>

// TODO muzeme pouzivat i udalosti a pomoci nich si synchronizovat vzdalene
// entity managery...
// (mozna).

/**
 * Here we define which components we use.
 * We use inheritance here to create a new type,
 * so that we could forward declare it when needed.
 */
struct EntityComponentSystem
    : public BaseEntityComponentSystem<Shape, Position, Explosive,
                                       geometry::Object2D, EntityMotion,
                                       PlayerInfo, Health> {};

using AnyComponent = typename EntityComponentSystem::AnyComponent;

using entity_t = typename EntityComponentSystem::entity_t;
