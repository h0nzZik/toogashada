#include <iostream>
#include <chrono>
#include <thread>
#include <set>
#include <memory>

#include <ctime>
#include <csignal>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "ConnectionToClient.hpp"
#include "Messages.h"

using namespace std;
using namespace std::chrono_literals;
using boost::asio::ip::tcp;

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

class tcp_server : private IConnection::IHandler
{
	public:
		tcp_server(int port)
			: acceptor(io_service, tcp::endpoint(tcp::v4(), port))
		{
			start_accept();
		}

		~tcp_server();

		void run() {
			io_service.run();
		}

		void shutdown() {
			io_service.stop();
			// TODO implement this
		}

	private:

		/* < IConnection::IHandler > */
		void received(IConnection & connection, Message msg) override;
		void disconnected(IConnection & connection) override;
		/* < IConnection::IHandler > */

		void start_accept()
		{
			auto conn = new ConnectionToClient(acceptor.get_io_service());

			// Capturing unique pointers is not easy
			acceptor.async_accept(conn->socket(),
					[this, conn](const boost::system::error_code & error){

					if (!error)
					{
						cout << "Client " << conn->socket().remote_endpoint() << " connected." << endl;
						connections.insert(conn);

						conn->listen(*this);
						// Send some hello message, followed by ping (TODO)
						conn->send(Message{Tag::Hello, {1,2,3}});
						send_him_a_few_polygons(*conn);
					} else {
						delete conn;
					}

					start_accept();
			});
		}

		void send_him_a_few_polygons(ConnectionToClient & client) {
			MsgNewPolygonalObject npo;
			npo.object_id = 25;
			npo.points.push_back(IntPoint{40, 40});
			npo.points.push_back(IntPoint{40, 120});
			npo.points.push_back(IntPoint{120, 40});
			client.send(npo.to_message());
		}

		boost::asio::io_service io_service;
		tcp::acceptor acceptor;

		// We do not use smart pointers to ease removal of items from set
		std::set<ConnectionToClient *> connections;
};

tcp_server::~tcp_server() {
	for (ConnectionToClient * conn : connections)
		delete conn;
}

void tcp_server::received(IConnection & connection, Message /*msg*/) {
	auto & conn = dynamic_cast<ConnectionToClient &>(connection);
	cout << "Server received message from " << conn.socket().remote_endpoint() << endl;	
}

void tcp_server::disconnected(IConnection & connection) {
	auto & conn = dynamic_cast<ConnectionToClient &>(connection);
	cout << "Client " << conn.socket().remote_endpoint() << " disconnected." << endl;

	connections.erase(&conn);
}


static tcp_server * p_server = nullptr;

static void on_signal(int signal) {
	if (!p_server)
		return;

	cout << "Signal " << signal << " caught." << endl;
	p_server->shutdown();
}

void server(int port)
{
	tcp_server server(port);
	p_server = &server;
	std::signal(SIGINT, on_signal);

	server.run();
	cout << "Server exited normally" << endl;
}

