// Standard
#include <stdexcept>
#include <memory>
#include <iostream>
#include <string>
#include <thread>

// Boost
#include <boost/asio.hpp>


// Project
#include "Tag.h"
#include "Message.h"
#include "Messages.h"
#include "ConnectionToServer.h"
#include "ClientGui.h"

using namespace std;

class TwoDimObject;

class ObjectManager {
	public:
		TwoDimObject & getObjectById(uint32_t id);
	private:
		std::map<uint32_t, TwoDimObject> _objects;
};

class Client : private IConnection::IHandler {
	public:
		Client();
		~Client() = default;

		void run();
	private:
		/* < IConnection::IHandler > */
		void received(IConnection & connection, Message msg) override;
		void disconnected(IConnection & connection) override;
		/* < IConnection::IHandler > */

		ClientGui gui;
		ConnectionToServer conn;
};


void Client::received(IConnection & connection, Message msg) {
	auto & conn = dynamic_cast<ConnectionToServer &>(connection);
	cout << "Client received message from " << conn.socket().remote_endpoint() << endl;	
	gui.on_message(std::move(msg));
}

void Client::disconnected(IConnection & connection) {
	auto & conn = dynamic_cast<ConnectionToServer &>(connection);
	cout << "Client " << conn.socket().remote_endpoint() << " disconnected." << endl;
}

Client::Client() :
	conn{"localhost", "2061"}
{
	;
}

void Client::run() {
	conn.listen(*this);
	conn.send(Message{Tag::Hello, {4, 5, 6}});
	
	thread network([&]{
			conn.run();
	});
	gui.run();
}

int main()
{
	cout << "Client\n";
	Client client;
	client.run();
}

