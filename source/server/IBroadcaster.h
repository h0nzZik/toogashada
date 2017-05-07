#pragma once

#include <common/BaseEntityComponentSystem.h>
#include <common/EntityComponentSystem.h>
#include <common/Message.h>

class IBroadcaster {
public:
  IBroadcaster() = default;
  virtual ~IBroadcaster() = default;

  virtual void iter() = 0;
  virtual void broadcast(Message message) = 0;
};
