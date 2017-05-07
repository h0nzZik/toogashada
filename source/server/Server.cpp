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

#include <server/Connection2Entity.h>
#include <server/ConnectionToClient.hpp>
#include <server/Connections.h>
#include <server/GameModel.h>
#include <server/IBroadcaster.h>
#include <server/SEntity.h>

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

  ~Impl() = default;
  void run() { gameModel.main(); }

  void shutdown() { gameModel.stop(); }

private:
  class Receiver;
  void received(ConnectionToClient &connection, ClientMessage msg);

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
    m_connections.erase(conn);
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
    m_connections.for_each([&](ConnectionToClient &cl) { cl.send(msg); });
  }

  void clearConnections();

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
            m_connections.insert(move(waiting_connection));
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
  Connections m_connections;
  Connection2Entity m_connection2entity;
  EntityComponentSystem ecs;
  GameModel gameModel;
};

class Server::Impl::Receiver : public boost::static_visitor<void> {
public:
  explicit Receiver(Impl &self, ConnectionToClient &connection)
      : self(self), connection(connection) {}

  void operator()(MsgIntroduceMyPlayer const &msg) {

    bool const has =
        self.m_connection2entity.do_for(connection, [&](entity_t /*entity*/) {
          cerr << "Client " << connection.socket().remote_endpoint()
               << " already introduced. Introduction ignored!" << endl;
        });
    if (has)
      return;

    // new logic
    self.sendAllEntities(connection);

    SEntity sentity = self.gameModel.newPlayer(msg.playerInfo);
    self.m_connection2entity.insert(connection, sentity.entity);

    // misc info for player
    MsgGameInfo msgGameInfo = {self.gameModel.getGameInfo()};
    self.broadcast({msgGameInfo});

    MsgNewEntity msgNewEntity;
    msgNewEntity.components =
        EntityComponentSystem::all_components(sentity.entity);
    msgNewEntity.entity_id = sentity.entity.get_component<EntityID>();

    MsgPlayerAssignedEntityId msgAssignedEntityId = {msgNewEntity.entity_id};
    self.broadcast({msgNewEntity});
    self.send(connection, {msgAssignedEntityId});
  }

  void operator()(MsgPlayerRotation const &msg) {
    self.gameModel.playerRotatesTo(
        {self.m_connection2entity.getEntity(connection)}, msg.rotation);
  }

  void operator()(MsgPlayerActionChange const &msg) {
    self.gameModel.playerKeyPress(
        {self.m_connection2entity.getEntity(connection)}, msg.movement,
        bool(msg.state));
  }

private:
  Impl &self;
  ConnectionToClient &connection;
};

void Server::Impl::received(ConnectionToClient &connection, ClientMessage msg) {
  Receiver receiver{*this, connection};
  boost::apply_visitor(receiver, msg.data);
}

void Server::Impl::clearConnections() {
  m_connections.handle_erase([&](ConnectionToClient &toClear) {
    m_connection2entity.do_for(toClear, [&](entity_t entity) {
      m_connection2entity.erase(toClear);
      gameModel.removeEntity({entity});
    });
  });
}

/***********************************/
/* Outer                           */
/***********************************/

Server::Server(int port) { pImpl = make_unique<Impl>(port); }

Server::~Server() = default;

void Server::run() { pImpl->run(); }

void Server::shutdown() { pImpl->shutdown(); }
