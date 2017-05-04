#pragma once

#include <memory>

class Server {
public:
  explicit Server(int port);
  ~Server();

  void run();
  void shutdown();

private:
  class Impl;
  std::unique_ptr<Impl> pImpl;
};
