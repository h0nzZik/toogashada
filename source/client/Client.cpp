#include <iostream>
#include <thread>
#include <utility> // std::move

#include <common/ClientMessage.h>
#include <common/Messages.h>
#include <common/Tag.h>

#include "ClientController.h"
#include "ClientGui.h"
#include "common/components/PlayerInfo.h"
#include "ConnectionToServer.h"
#include "RemoteServerWrapper.h"

#include "Client.h"

using namespace std;

class Client::Impl : private IConnection::IHandler {
public:
  Impl(std::string ip, std::string port, std::string playerName,
       std::string playerTeam);

  ~Impl() = default;

  void run();

private:
  /* < IConnection::IHandler > */
  void received(IConnection &connection, Message msg) override;
  void disconnected(IConnection &connection) override;
  /* < IConnection::IHandler > */

  ClientGui clientGui;
  ConnectionToServer serverConnection;
  RemoteServerWrapper remoteServerWrapper;
  ClientController clientController;
  PlayerInfo playerInfo;
};

Client::~Client() = default;

Client::Client(string ip, string port, std::string playerName,
               std::string playerTeam) {
  impl = make_unique<Impl>(move(ip), move(port), move(playerName),
                           move(playerTeam));
}

void Client::run() { impl->run(); }

Client::Impl::Impl(string ip, string port, std::string playerName,
                   std::string playerTeam)
    : clientGui{clientController, playerName, playerTeam},
      serverConnection{ip, port}, remoteServerWrapper{serverConnection},
      clientController{playerInfo, clientGui, remoteServerWrapper},
      playerInfo{playerName, playerTeam} {
  ;
}

void Client::Impl::received(IConnection &connection, Message msg) {
  // auto & conn = dynamic_cast<ConnectionToServer &>(connection);
  // cout << "Client received message from " << conn.socket().remote_endpoint()
  // << endl;

  clientController.received(std::move(msg));
}

void Client::Impl::disconnected(IConnection &connection) {
  auto &conn = dynamic_cast<ConnectionToServer &>(connection);
  cout << "Server " << conn.socket().remote_endpoint() << " disconnected."
       << endl;
  clientController.stop();
}

void Client::Impl::run() {
  serverConnection.listen(*this);

  ClientMessage msg{
      MsgIntroduceMyPlayer{playerInfo}};
  serverConnection.send(msg.to_message());

  thread network([&] { serverConnection.run(); });
  clientController.main_loop();

  serverConnection.stop();
  network.join();
}
