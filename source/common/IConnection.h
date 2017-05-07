#pragma once

#include <functional>

#include "Message.h"

class IConnection {
public:
  virtual ~IConnection() = default;
  virtual void send(Message message) = 0;

  class IHandler;
  virtual void listen(IHandler &handler) = 0;
};

class IConnection::IHandler {
public:
  virtual ~IHandler() = default;

  virtual void received(IConnection &connection, Message msg) = 0;
  virtual void disconnected(IConnection &connection) = 0;
};
