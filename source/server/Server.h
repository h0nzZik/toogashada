#pragma once

#include <set>
#include <boost/asio.hpp>

#include <common/IConnection.h>
#include <common/Messages.h>
#include <common/GameObjectManager.h>
#include <common/PolygonalShape.h>
#include <server/IBroadcaster.h>
#include "ConnectionToClient.hpp"
#include "GameModel.h"
#include "PlayerManager.h"

class Server : private IConnection::IHandler, private IBroadcaster
{
	public:
		explicit Server(int port);
		~Server();

		void run();
		void shutdown();

	private:

		/* < IConnection::IHandler > */
		void received(IConnection & connection, Message msg) override;
		void disconnected(IConnection & connection) override;
		/* < IConnection::IHandler > */

		/* < IBroadcaster > */
		void broadcast(Message msg) override;
		void notify(GameObject const & gameObject) override;
		/* </IBroadcaster > */

		void start_accept();
		void newClientConnected(ConnectionToClient & client);

		static Vector toVector(Player::Movement movement);
		static Message createMessage_NewGameObject(GameObject const & object);
		static Message createMessage_NewObjectPosition(GameObject const & object);
		static Message createMessage_NewPlayer(Player const & player);
		void playerMoves(ConnectionToClient & client, Player::Movement movement);
		void send_him_a_few_polygons(ConnectionToClient & client);
		void init_player_shape();

		boost::asio::io_service io_service;
		boost::asio::ip::tcp::acceptor acceptor;

		// We do not use smart pointers to ease removal of items from set
		std::set<ConnectionToClient *> connections;
		std::map<ConnectionToClient const *, Player *> connection2player;
		GameObjectManager gameObjects;
		PlayerManager players;
		IntPolygonalShape playerShape;
		GameModel gameModel;
};


