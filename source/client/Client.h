#ifndef SRC_CLIENT_H_
#define SRC_CLIENT_H_

// Standard
#include <memory>
#include <string>

// Client
#include "ClientOptions_fwd.h"

class Client {
public:
  Client(ClientOptions const &options);
  ~Client();

  void run();

private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};

#endif /* SRC_CLIENT_H_ */
