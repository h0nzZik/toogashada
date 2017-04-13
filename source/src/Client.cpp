#include <iostream>
#include <thread>
#include "Client.h"
#include "Tag.h"

using namespace std;

Client::Client() :
	clientGui{},
	serverConnection{"localhost", "2061"},
	remoteServerWrapper{serverConnection},
	clientController{clientGui, remoteServerWrapper}
{
	;
}

void Client::received(IConnection & connection, Message msg) {
	auto & conn = dynamic_cast<ConnectionToServer &>(connection);
	cout << "Client received message from " << conn.socket().remote_endpoint() << endl;

	clientController.received(std::move(msg));
}

void Client::disconnected(IConnection & connection) {
	auto & conn = dynamic_cast<ConnectionToServer &>(connection);
	cout << "Client " << conn.socket().remote_endpoint() << " disconnected." << endl;
}

void Client::run() {
	serverConnection.listen(*this);
	serverConnection.send(Message{Tag::Hello, {4, 5, 6}});

	thread network([&]{
		serverConnection.run();
	});
	clientController.main_loop();
}
