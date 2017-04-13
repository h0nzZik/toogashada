#include <iostream>


#include "Server.h"

using namespace std;
using boost::asio::ip::tcp;

Server::Server(int port)
	: acceptor(io_service, tcp::endpoint(tcp::v4(), port))
{
	start_accept();
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

void Server::received(IConnection & connection, Message /*msg*/) {
	auto & conn = dynamic_cast<ConnectionToClient &>(connection);
	cout << "Server received message from " << conn.socket().remote_endpoint() << endl;	
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
				connections.insert(conn);

				conn->listen(*this);
				newClientConnected(*conn);
			} else {
				delete conn;
			}

			start_accept();
	});
}

void Server::newClientConnected(ConnectionToClient & client) {
	client.send(Message{Tag::Hello, {1,2,3}});
	send_him_a_few_polygons(client);

}

void Server::send_him_a_few_polygons(ConnectionToClient & client) {
	MsgNewPolygonalObject npo;
	npo.object_id = 25;
	npo.shape.push_back(IntPoint{40, 40});
	npo.shape.push_back(IntPoint{40, 120});
	npo.shape.push_back(IntPoint{70, 140});
	npo.shape.push_back(IntPoint{140,140});
	client.send(npo.to_message());
}

