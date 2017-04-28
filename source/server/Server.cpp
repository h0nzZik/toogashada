#include <iostream>
#include <sstream>

#include <boost/variant/static_visitor.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/types/boost_variant.hpp>
#include <cereal/types/vector.hpp>

#include <entityplus/entity.h>

#include <common/ServerMessage.h>
#include <common/ClientMessage.h>
#include <common/Messages.h>

#include <common/EntityComponentSystem.h>


#include "Server.h"

using namespace std;
using boost::asio::ip::tcp;

// TODO pImpl

Server::Server(int port) :
		acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
		gameModel(gameObjects, ecs, *this)
{
	start_accept();
	init_player_shape();
}

Server::~Server() {
	for (ConnectionToClient * conn : connections)
		delete conn;
}

void Server::run() {
	io_service.run();
}

void Server::shutdown() {
	io_service.stop();
	// TODO implement this
}

class Server::Receiver : public boost::static_visitor<void> {
public:
	explicit Receiver(Server & self, ConnectionToClient & connection) :
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
	Server & self;
	ConnectionToClient & connection;
};


void Server::received(ConnectionToClient & connection, ClientMessage msg) {
	Receiver receiver{*this, connection};
	boost::apply_visitor(receiver, msg.data);
}


void Server::received(IConnection & connection, Message msg) {
	auto & conn = dynamic_cast<ConnectionToClient &>(connection);
	cout << "Server received message from " << conn.socket().remote_endpoint() << endl;
	switch(msg.tag) {
	case Tag::UniversalClientMessage:
		return received(conn, ClientMessage::from(msg));

	default:
		break;
	}
}

void Server::playerMoves(ConnectionToClient & client, Player::Movement movement) {
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

Vector Server::toVector(Player::Movement movement) {
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

void Server::disconnected(IConnection & connection) {
	auto & conn = dynamic_cast<ConnectionToClient &>(connection);
	cout << "Client " << conn.socket().remote_endpoint() << " disconnected." << endl;

	connections.erase(&conn);
}

void Server::start_accept() {
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

void Server::init_player_shape() {
	playerShape.clear();
	playerShape.push_back({-10, 0});
	playerShape.push_back({-20, -10});
	playerShape.push_back({+20, -10});
	playerShape.push_back({+10, 0});
}

// TODO we will need to have some area size.

void Server::newClientConnected(ConnectionToClient & client) {
	client.send(Message{Tag::Hello, {1,2,3}});
	//send_him_a_few_polygons(client);

	// TODO create a new player. We will need some manager of players.
	uint32_t const object_id = gameObjects.get_fresh_id();
	auto _object = make_unique<GameObject>(object_id);
	GameObject & object = *_object;
	gameObjects.insert(std::move(_object));
	object.shape = playerShape;
	object.center = {Scalar(rand() % 200), Scalar(rand() % 200)};

	auto const player_id = players.get_fresh_id();
	auto _player = make_unique<Player>(player_id, object);
	Player & player = *_player;
	players.insert(std::move(_player));
	//connection2player[&client] = &player;

	// Send diffs to all current players.
	// New player must not be in the set yet.
	broadcast(createMessage_NewGameObject(object));
	broadcast(createMessage_NewPlayer(player));

	// Send whole game state to the new player
	for (GameObject const & object : gameObjects) {
		send(client, createMessage_NewGameObject(object));
	}

	for (Player const & player : players) {
		send(client, createMessage_NewPlayer(player));
	}

	// Some experiments with EntityComponent
	// Opravdu bych mohl pridat komponentu pro identitu entit...?
	entity_t entity = ecs.entityManager.create_entity(EntityID::newID());
	Position pos;
	pos.center = object.center;
	entity.add_component<Position>(pos);
	//entity.add_component<Shape>(CircleShape{5.1}); // for some reazon it is zero on client
	entity.add_component<Shape>(playerShape);
	MsgNewEntity msg;
	connection2entity[&client] = entity;
	msg.components = EntityComponentSystem::all_components(entity);
	msg.entity_id = entity.get_component<EntityID>();
	send(client, {msg});
	broadcast({msg});

	// Taky musime mit PlayerComponent
	// Budeme mit zpravu, ktera umi rict 'Nova entita' a seznam komponent.
	// My chceme updatovat
}

ServerMessage Server::createMessage_NewGameObject(GameObject const & object) {
	MsgNewPolygonalObject npo;
	npo.object_id = object.id();
	npo.center = object.center;
	npo.shape = object.shape;
	return ServerMessage {npo};
}

ServerMessage Server::createMessage_NewObjectPosition(GameObject const & object) {
	MsgObjectPosition mop;
	mop.object_id = object.id();
	mop.new_center = object.center;
	return ServerMessage {mop};
}

ServerMessage Server::createMessage_NewPlayer(Player const & player) {
	MsgNewPlayer mnp;
	mnp.player_id = player.id();
	mnp.object_id = player.gameObject().id();
	mnp.playerName = player.name;
	return ServerMessage {mnp};
}

void Server::send(ConnectionToClient & client, ServerMessage const & msg) {
	auto m = msg.to_message();
	client.send(m);
}

void Server::broadcast(Message msg) {
	// A copy of message is created for every connection,
	// which is not really optimal. We may optimize that later.
	for (ConnectionToClient * cl : connections) {
		cl->send(msg);
	}
}

void Server::send_him_a_few_polygons(ConnectionToClient & client) {
	MsgNewPolygonalObject npo;
	npo.object_id = 25;
	npo.shape.push_back(Point{40, 40});
	npo.shape.push_back(Point{40, 120});
	npo.shape.push_back(Point{70, 140});
	npo.shape.push_back(Point{140,140});
	ServerMessage usm{npo};
	client.send(usm.to_message());
}

void Server::notify(GameObject const & gameObject) {
	broadcast(createMessage_NewObjectPosition(gameObject));
}

void Server::notify(entity_t const &entity, AnyComponent const &component) {
	MsgUpdateEntity mue;
	mue.entity_id = entity.get_component<EntityID>();
	mue.components = {component};
	ServerMessage usm{mue};
	broadcast(usm);
}

void Server::broadcast(ServerMessage const & msg) {
	broadcast(msg.to_message());
}

