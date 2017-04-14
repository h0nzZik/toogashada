#include <iostream>


#include "Server.h"

using namespace std;
using boost::asio::ip::tcp;

Server::Server(int port) :
		acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
		gameModel(gameObjects, *this)
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

void Server::received(IConnection & connection, Message msg) {
	auto & conn = dynamic_cast<ConnectionToClient &>(connection);
	cout << "Server received message from " << conn.socket().remote_endpoint() << endl;
	switch(msg.tag) {
	case Tag::PlayerMovesBackward:
		return playerMoves(conn, Player::Movement::Backward);
	case Tag::PlayerMovesForward:
		return playerMoves(conn, Player::Movement::Forward);
	case Tag::PlayerMovesLeft:
		return playerMoves(conn, Player::Movement::Left);
	case Tag::PlayerMovesRight:
		return playerMoves(conn, Player::Movement::Right);
	case Tag::PlayerStops:
		return playerMoves(conn, Player::Movement::None);

	default:
		break;
	}
}

void Server::playerMoves(ConnectionToClient & client, Player::Movement movement) {
	if (connection2player[&client] == nullptr) {
		cerr << "[warn] player does not exist, but moves" << endl;
		return;
	}

	Player & player = *connection2player[&client];
	// This is a bit naive, but as a first experiment
	player.gameObject().speed = toVector(movement);
	//player.gameObject().center += toVector(movement);
	broadcast(createMessage_NewObjectPosition(player.gameObject()));
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
	connection2player[&client] = &player;

	// Send diffs to all current players.
	// New player must not be in the set yet.
	broadcast(createMessage_NewGameObject(object));
	broadcast(createMessage_NewPlayer(player));

	// Send whole game state to the new player
	for (GameObject const & object : gameObjects) {
		client.send(createMessage_NewGameObject(object));
	}

	for (Player const & player : players) {
		client.send(createMessage_NewPlayer(player));
	}
}

Message Server::createMessage_NewGameObject(GameObject const & object) {
	MsgNewPolygonalObject npo;
	npo.object_id = object.id();
	npo.center = object.center;
	npo.shape = object.shape;
	return npo.to_message();
}

Message Server::createMessage_NewObjectPosition(GameObject const & object) {
	MsgObjectPosition mop;
	mop.object_id = object.id();
	mop.new_center = object.center;
	return mop.to_message();
}

Message Server::createMessage_NewPlayer(Player const & player) {
	MsgNewPlayer mnp;
	mnp.player_id = player.id();
	mnp.object_id = player.gameObject().id();
	mnp.player_name = player.name;
	return mnp.to_message();
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
	client.send(npo.to_message());
}

void Server::notify(GameObject const & gameObject) {
	broadcast(createMessage_NewObjectPosition(gameObject));
}

