#pragma once

#include <functional> // function
#include <memory>     // unique_ptr

class ConnectionToClient;

class Connections {
public:
  Connections();
  ~Connections();

  ConnectionToClient &insert(std::unique_ptr<ConnectionToClient> connection);
  void erase(ConnectionToClient &connection);
  void for_each(std::function<void(ConnectionToClient &)> f);
  void handle_erase(std::function<void(ConnectionToClient &)> f);

private:
  class Impl;
  std::unique_ptr<Impl> pImpl;
};
