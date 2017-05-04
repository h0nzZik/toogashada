#ifndef SRC_CLIENT_H_
#define SRC_CLIENT_H_

#include <memory>
#include <string>

class Client {
public:
  Client(std::string ip, std::string port, std::string playerName,
         std::string playerTeam);
  ~Client();

  void run();

private:
  class Impl;
  std::unique_ptr<Impl> impl;
};

#endif /* SRC_CLIENT_H_ */
