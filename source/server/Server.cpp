#include <iostream>
#include <sstream>
#include <set>

#include <boost/asio.hpp>
#include <boost/variant/static_visitor.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/types/boost_variant.hpp>
#include <cereal/types/vector.hpp>

#include <entityplus/entity.h>

#include <common/ClientMessage.h>
#include <common/IConnection.h>
#include <common/Messages.h>
#include <common/Geometry.h>
#include <common/ServerMessage.h>
#include <common/EntityComponentSystem.h>

#include <server/IBroadcaster.h>
#include "ConnectionToClient.hpp"
#include "GameModel.h"
#include "Player.h"
#include "SEntity.h"

#include "Server.h"

using namespace std;
using boost::asio::ip::tcp;

class Server::Impl : private IConnection::IHandler, private IBroadcaster
{
public:
	explicit Impl(int port) :
	acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
	gameModel(ecs, *this)
	{
		// A circle
		entity_t entity = ecs.entityManager.create_entity(EntityID::newID());
		entity.add_component<Shape>(geometry::CircleShape{10});
		Position pos;
		pos.speed = {0,0};
		pos.center = geometry::Point{30, 30};
		pos.angularSpeed = 0;
		pos.rotation = 0;
		entity.add_component<Position>(pos);
		entity.add_component<geometry::Object2D>(geometry::createObject2D(
				pos.center, 0,entity.get_component<Shape>()));

		start_accept();
	}

	~Impl() {
		for (ConnectionToClient * conn : connections)
			delete conn;
	}

	void run() {
		io_service.run();
	}

	void shutdown() {
		io_service.stop();
		// TODO implement this
	}

private:

	class Receiver;
	void received(ConnectionToClient & connection, ClientMessage msg) {
		Receiver receiver{*this, connection};
		boost::apply_visitor(receiver, msg.data);
	}

	class Receiver : public boost::static_visitor<void> {
	public:
		explicit Receiver(Impl & self, ConnectionToClient & connection) :
		self(self),
		connection(connection)
		{

		}

		void operator()(MsgIntroduceMyPlayer const /*& msg*/) {

		}

		void operator()(MsgPlayerMovesLeft const & /*msg*/) {
			self.playerMoves(connection, Player::Movement::Left);
		}

		void operator()(MsgPlayerMovesRight const & /*msg*/) {
			self.playerMoves(connection, Player::Movement::Right);
		}

		void operator()(MsgPlayerMovesForward const & /*msg*/) {
			self.playerMoves(connection, Player::Movement::Forward);
		}

		void operator()(MsgPlayerMovesBackward const & /*msg*/) {
			self.playerMoves(connection, Player::Movement::Backward);
		}

		void operator()(MsgPlayerStops const & /*msg*/) {
			self.playerMoves(connection, Player::Movement::None);
		}

		void operator()(MsgPlayerShoots const & /*msg*/) {
			/* generate new random moving object */
			// TODO refactor to game model
			self.gameModel.newBullet();
		}

		void operator()(MsgPlayerRotation const & /*msg*/) {

			//TODO replace multiple messages with this one for key handling
		}

		void operator()(MsgPlayerActionChange const & /*msg*/) {

			// TODO player angle
		}

	private:
		Impl & self;
		ConnectionToClient & connection;
	};

	/* < IConnection::IHandler > */
	void received(IConnection & connection, Message msg) override {
		auto & conn = dynamic_cast<ConnectionToClient &>(connection);
		//cout << "Server received message from " << conn.socket().remote_endpoint() << endl;
		switch(msg.tag) {
		case Tag::UniversalClientMessage:
			return received(conn, ClientMessage::from(msg));

		default:
			break;
		}
	}

	void disconnected(IConnection & connection) override {
		auto & conn = dynamic_cast<ConnectionToClient &>(connection);
		cout << "Client " << conn.socket().remote_endpoint() << " disconnected." << endl;

		entity_t entity = connection2entity.at(&conn);
		connection2entity.erase(&conn);
		connections.erase(&conn);
		delete &conn;

		gameModel.removeEntity({entity});
	}
	/* </IConnection::IHandler > */

	void send(ConnectionToClient & client, ServerMessage const & msg) {
		auto m = msg.to_message();
		client.send(m);

	}
	void broadcast(ServerMessage const & msg) {
		broadcast(msg.to_message());
	}
	/* < IBroadcaster > */
	void broadcast(Message msg) override {
		// A copy of message is created for every connection,
		// which is not really optimal. We may optimize that later.
		for (ConnectionToClient * cl : connections) {
			cl->send(msg);
		}

	}

	void updateEntity(entity_t const & entity, AnyComponent const &component) override {
		MsgUpdateEntity mue;
		mue.entity_id = entity.get_component<EntityID>();
		mue.components = {component};
		ServerMessage usm{mue};
		broadcast(usm);
	}
	/* </IBroadcaster > */

	void start_accept() {
		auto conn = new ConnectionToClient(acceptor.get_io_service());

		// Capturing unique pointers is not easy
		acceptor.async_accept(conn->socket(),
				[this, conn](const boost::system::error_code & error){

			if (!error)
			{
				cout << "Client " << conn->socket().remote_endpoint() << " connected." << endl;
				conn->listen(*this);
				// The client is added later, so we can broadcast all changes collectively
				// and send him full state individually.
				newClientConnected(*conn);
				connections.insert(conn);
			} else {
				delete conn;
			}

			start_accept();
		});
	}

	void newClientConnected(ConnectionToClient & client) {
		//client.send(Message{Tag::Hello, {1,2,3}});

		sendAllEntities(client);
		SEntity sentity = gameModel.newPlayer();

		connection2entity[&client] = sentity.entity;

		MsgNewEntity msg;
		msg.components = EntityComponentSystem::all_components(sentity.entity);
		msg.entity_id = sentity.entity.get_component<EntityID>();
		send(client, {msg});
		broadcast({msg});

		// Taky musime mit PlayerComponent
	}

	void sendAllEntities(ConnectionToClient & client) {
		ecs.entityManager.for_each<EntityID>([&](entity_t entity, EntityID const & id){
			MsgNewEntity msg;
			msg.entity_id = id;
			msg.components = EntityComponentSystem::all_components(entity);
			send(client, {msg});
		});
	}

	static geometry::Vector toVector(Player::Movement movement) {
		switch(movement) {
		case Player::Movement::Left:
			return {-15, 0};
		case Player::Movement::Right:
			return {+15, 0};
		case Player::Movement::Backward:
			return {0, +15};
		case Player::Movement::Forward:
			return {0, -45};

		case Player::Movement::None:
			return {0, 0};
		default:
			return {0, 0}; // exception might be nicer
		}
	}

	void playerMoves(ConnectionToClient & client, Player::Movement movement) {
		auto it = connection2entity.find(&client);
		if (it == connection2entity.end()) {
			cerr << "[warn] player does not exist, but moves" << endl;
			return;
		}

		entity_t player = it->second;
		//Player & player = *connection2player[&client];
		//entity_t player = *connection2entity[&client];
		player.sync();
		if (player.has_component<Position>()) {
			auto newSpeed = toVector(movement);
			//cout << "New speed: " << newSpeed << endl;
			player.get_component<Position>().speed = newSpeed;
		}


		//player.gameObject().center += toVector(movement);
		//broadcast(createMessage_NewObjectPosition(player.gameObject()));
	}

	boost::asio::io_service io_service;
	boost::asio::ip::tcp::acceptor acceptor;

	// We do not use smart pointers to ease removal of items from set
	std::set<ConnectionToClient *> connections;
	std::map<ConnectionToClient const *, entity_t> connection2entity;
	EntityComponentSystem ecs;
	GameModel gameModel;
};

// TODO we will need to have some area size.

Server::Server(int port) {
	pImpl = make_unique<Impl>(port);
}

Server::~Server() = default;

void Server::run() {
	pImpl->run();
}

void Server::shutdown() {
	pImpl->shutdown();
}
