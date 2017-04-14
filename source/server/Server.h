#pragma once

#include <set>
#include <boost/asio.hpp>

#include <common/IConnection.h>
#include <common/Messages.h>
#include <common/GameObjectManager.h>
#include <common/PolygonalShape.h>

#include "ConnectionToClient.hpp"
#include "PlayerManager.h"

class Server : private IConnection::IHandler
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
		void broadcast(Message msg);

		void start_accept();
		void newClientConnected(ConnectionToClient & client);

		Message createMessage_NewGameObject(GameObject const & object);
		Message createMessage_NewPlayer(Player const & player);
		void send_him_a_few_polygons(ConnectionToClient & client);
		void init_player_shape();

		boost::asio::io_service io_service;
		boost::asio::ip::tcp::acceptor acceptor;

		// We do not use smart pointers to ease removal of items from set
		std::set<ConnectionToClient *> connections;
		GameObjectManager gameObjects;
		PlayerManager players;
		IntPolygonalShape playerShape;
};


