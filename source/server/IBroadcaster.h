#pragma once

#include <common/Message.h>

class GameObject;

class IBroadcaster {
public:
	IBroadcaster() = default;
	virtual ~IBroadcaster() = default;

	virtual void broadcast(Message message) = 0;
	virtual void notify(GameObject const & gameObject) = 0;
};
