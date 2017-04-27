#include <iostream>
#include <thread>

#include <common/Tag.h>
#include <common/Messages.h>
#include <common/ClientMessage.h>

#include "Client.h"

using namespace std;

Client::Client(string ip, string port, std::string playerName, std::string playerTeam) :
	clientGui{},
	serverConnection{ip, port},
	remoteServerWrapper{serverConnection},
	clientController{clientPlayer, clientGui, remoteServerWrapper},
    clientPlayer{playerName, playerTeam}
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

    ClientMessage msg{MsgIntroduceMyPlayer{clientPlayer.mName, clientPlayer.mTeam}};
	serverConnection.send(msg.to_message());


	thread network([&]{
		serverConnection.run();
	});
	clientController.main_loop();
}
