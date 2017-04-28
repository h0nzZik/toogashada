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
#include <common/PolygonalShape.h>
#include <common/ServerMessage.h>
#include <common/EntityComponentSystem.h>

#include <server/IBroadcaster.h>
#include "ConnectionToClient.hpp"
#include "GameModel.h"
#include "Player.h"

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
		start_accept();
		init_player_shape();
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

	private:
		Impl & self;
		ConnectionToClient & connection;
	};

	/* < IConnection::IHandler > */
	void received(IConnection & connection, Message msg) override {
		auto & conn = dynamic_cast<ConnectionToClient &>(connection);
		cout << "Server received message from " << conn.socket().remote_endpoint() << endl;
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

		connections.erase(&conn);
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

	void notify(entity_t const & entity, AnyComponent const &component) override {
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

		entity_t entity = ecs.entityManager.create_entity(EntityID::newID());
		Position pos;
		pos.center = {Scalar(rand() % 200), Scalar(rand() % 200)};
		entity.add_component<Position>(pos);
		//entity.add_component<Shape>(CircleShape{5.1});
		entity.add_component<Shape>(playerShape);

		MsgNewEntity msg;
		connection2entity[&client] = entity;
		msg.components = EntityComponentSystem::all_components(entity);
		msg.entity_id = entity.get_component<EntityID>();
		send(client, {msg});
		broadcast({msg});

		// Taky musime mit PlayerComponent
	}

	static Vector toVector(Player::Movement movement) {
		switch(movement) {
		case Player::Movement::Left:
			return {-5, 0};
		case Player::Movement::Right:
			return {+5, 0};
		case Player::Movement::Backward:
			return {0, -5};
		case Player::Movement::Forward:
			return {0, +15};

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
			cout << "New speed: " << newSpeed << endl;
			player.get_component<Position>().speed = newSpeed;
		}


		//player.gameObject().center += toVector(movement);
		//broadcast(createMessage_NewObjectPosition(player.gameObject()));
	}

	void init_player_shape() {
		playerShape.clear();
		playerShape.push_back({-10, 0});
		playerShape.push_back({-20, -10});
		playerShape.push_back({+20, -10});
		playerShape.push_back({+10, 0});
	}

	boost::asio::io_service io_service;
	boost::asio::ip::tcp::acceptor acceptor;

	// We do not use smart pointers to ease removal of items from set
	std::set<ConnectionToClient *> connections;
	std::map<ConnectionToClient const *, entity_t> connection2entity;
	PolygonalShape playerShape;
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
