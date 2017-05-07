#include <iostream>
#include <set>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/variant/static_visitor.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/types/boost_variant.hpp>
#include <cereal/types/vector.hpp>

#include <entityplus/entity.h>

#include <common/ClientMessage.h>
#include <common/EntityComponentSystem.h>
#include <common/Geometry.h>
#include <common/IConnection.h>
#include <common/Messages.h>
#include <common/ServerMessage.h>

#include "ConnectionToClient.hpp"
#include "GameModel.h"
#include "SEntity.h"
#include <server/IBroadcaster.h>

#include "Server.h"

using namespace std;
using boost::asio::ip::tcp;

class Server::Impl : private IConnection::IHandler, private IBroadcaster {
public:
  explicit Impl(int port)
      : acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
        gameModel(ecs, *this) {
    start_accept();
  }

  ~Impl() {
    for (ConnectionToClient *conn : connections)
      delete conn;
  }

  void run() { gameModel.main(); }

  void shutdown() { gameModel.stop(); }

private:
  class Receiver;

  void received(ConnectionToClient &connection, ClientMessage msg) {
    Receiver receiver{*this, connection};
    boost::apply_visitor(receiver, msg.data);
  }

  class Receiver : public boost::static_visitor<void> {
  public:
    explicit Receiver(Impl &self, ConnectionToClient &connection)
        : self(self), connection(connection) {}

    void operator()(MsgIntroduceMyPlayer const &msg) {

      if (self.connection2entity.find(&connection) ==
          self.connection2entity.end()) {
        // new logic
        self.sendAllEntities(connection);

        SEntity sentity = self.gameModel.newPlayer(msg.playerInfo);
        self.connection2entity[&connection] = sentity.entity;

        // misc info for player
        MsgGameInfo msgGameInfo = {self.gameModel.getGameInfo()};
        self.broadcast({msgGameInfo});

        MsgNewEntity msgNewEntity;
        msgNewEntity.components =
            EntityComponentSystem::all_components(sentity.entity);
        msgNewEntity.entity_id = sentity.entity.get_component<EntityID>();

        MsgPlayerAssignedEntityId msgAssignedEntityId = {
            msgNewEntity.entity_id};
        self.broadcast({msgNewEntity});
        self.send(connection, {msgAssignedEntityId});

      } else {

        cerr << "Client " << connection.socket().remote_endpoint()
             << " already introduced. Introduction ignored!" << endl;
      }
    }

    void operator()(MsgPlayerRotation const &msg) {
      self.gameModel.playerRotatesTo({self.getEntity(connection)},
                                     msg.rotation);
    }

    void operator()(MsgPlayerActionChange const &msg) {
      self.gameModel.playerKeyPress({self.getEntity(connection)}, msg.movement,
                                    bool(msg.state));
    }

  private:
    Impl &self;
    ConnectionToClient &connection;
  };

  // TODO make standalone class from this
  entity_t getEntity(ConnectionToClient &connection) {
    entity_t &entity = connection2entity.at(&connection);
    entity.sync();
    return entity;
  }

  /* < IConnection::IHandler > */
  void received(IConnection &connection, Message msg) override {
    auto &conn = dynamic_cast<ConnectionToClient &>(connection);
    switch (msg.tag) {
    case Tag::UniversalClientMessage:
      return received(conn, ClientMessage::from(msg));

    default:
      break;
    }
  }

  void disconnected(IConnection &connection) override {
    auto &conn = dynamic_cast<ConnectionToClient &>(connection);
    connectionsToClear.insert(&conn);
  }

  /* </IConnection::IHandler > */

  void send(ConnectionToClient &client, ServerMessage const &msg) {
    auto m = msg.to_message();
    client.send(m);
  }

  void broadcast(ServerMessage const &msg) { broadcast(msg.to_message()); }

  /* < IBroadcaster > */
  void broadcast(Message msg) override {
    // A copy of message is created for every connection,
    // which is not really optimal. We may optimize that later.
    for (ConnectionToClient *cl : connections) {
      cl->send(msg);
    }
  }

  std::set<ConnectionToClient *> connectionsToClear;
  void clearConnections() {
    std::set<ConnectionToClient *> localConnectionsToClear;
    using std::swap;
    swap(localConnectionsToClear, connectionsToClear);

    // Clear them
    for (ConnectionToClient *toClear : localConnectionsToClear) {
      connections.erase(toClear);
    }

    // Clear associated entities
    for (ConnectionToClient *toClear : localConnectionsToClear) {
      auto it = connection2entity.find(toClear);
      if (it != connection2entity.end()) {
        entity_t entity = it->second;
        entity.sync();
        connection2entity.erase(toClear);
        gameModel.removeEntity({entity});
      }
    }

    for (ConnectionToClient *toClear : localConnectionsToClear) {
      delete toClear;
    }
  }

  void iter() override {
    io_service.poll();
    clearConnections();
  }

  /* </IBroadcaster > */

  unique_ptr<ConnectionToClient> waiting_connection;

  void start_accept() {
    waiting_connection =
        make_unique<ConnectionToClient>(acceptor.get_io_service());

    // Capturing unique pointers is not easy
    acceptor.async_accept(
        waiting_connection->socket(),
        [this](const boost::system::error_code &error) {

          if (!error) {
            cout << "Client " << waiting_connection->socket().remote_endpoint()
                 << " connected." << endl;
            waiting_connection->listen(*this);
            connections.insert(waiting_connection.release());
          } else {
            waiting_connection.reset();
          }

          start_accept();
        });
  }

  void sendAllEntities(ConnectionToClient &client) {
    ecs.entityManager.for_each<EntityID>(
        [&](entity_t entity, EntityID const &id) {
          MsgNewEntity msg;
          msg.entity_id = id;
          msg.components = EntityComponentSystem::all_components(entity);
          send(client, {msg});
        });
  }

  boost::asio::io_service io_service;
  boost::asio::ip::tcp::acceptor acceptor;

  // We do not use smart pointers to ease removal of items from set
  std::set<ConnectionToClient *> connections;
  std::map<ConnectionToClient const *, entity_t> connection2entity;
  EntityComponentSystem ecs;
  GameModel gameModel;
};

Server::Server(int port) { pImpl = make_unique<Impl>(port); }

Server::~Server() = default;

void Server::run() { pImpl->run(); }

void Server::shutdown() { pImpl->shutdown(); }
