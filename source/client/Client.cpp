// Standard
#include <iostream>
#include <thread>

// Common
#include <common/ClientMessage.h>

// Client
#include "ClientController.h"
#include "ClientGui.h"
#include "ClientOptions.h"
#include "ConnectionToServer.h"

// self
#include "Client.h"

using namespace std;

// Client Implementation Definition
class Client::Impl : private IConnection::IHandler {

public:
  Impl(ClientOptions const &options)
      : clientGui{clientController, options.playerName, options.playerTeam,
                  options.windowWidth, options.windowHeight},
        serverConnection{options.ip, options.port},
        clientController{{options.playerName, options.playerTeam},
                         clientGui,
                         serverConnection} {}

  ~Impl() = default;

  void run();

private:
  /* < IConnection::IHandler > */
  void received(IConnection &connection, Message msg) override;
  void disconnected(IConnection &connection) override;
  /* < IConnection::IHandler > */

  ClientGui clientGui;
  ConnectionToServer serverConnection;
  ClientController clientController;
};

void Client::Impl::received(IConnection & /*connection*/, Message msg) {

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

  clientController.start();

  ConnectionToServer::Statistics stats = serverConnection.getStatistics();
  cout << "Connection closed." << endl;
  cout << "Total bytes received: " << stats.bytes_received << endl;
  cout << "Total bytes transmitted: " << stats.bytes_sent << endl;
}

// Client Wrapper Definition
Client::Client(ClientOptions const &options)
    : mImpl(make_unique<Impl>(options)) {}
Client::~Client() = default;

void Client::run() { mImpl->run(); }
