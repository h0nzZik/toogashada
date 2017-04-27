#pragma once

#include <common/Message.h>
#include <common/EntityComponentSystem.h>

class GameObject;

struct AnyComponent;

class IBroadcaster {
public:
	IBroadcaster() = default;
	virtual ~IBroadcaster() = default;

	virtual void broadcast(Message message) = 0;
	virtual void notify(GameObject const & gameObject) = 0;
	virtual void notify(entity_t entity, AnyComponent const &component) = 0;
};
