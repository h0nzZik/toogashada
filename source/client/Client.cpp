#include <iostream>
#include <thread>

#include <common/ClientMessage.h>

#include "ClientController.h"
#include "ConnectionToServer.h"

#include "Client.h"

using namespace std;


// Client Implementation Definition
class Client::Impl : private IConnection::IHandler {

public:
	Impl(std::string ip, std::string port, std::string playerName,
	     std::string playerTeam, int windowWidth = -1, int windowHeight = -1);

	~Impl() = default;

	void run();

private:
	/* < IConnection::IHandler > */
	void received(IConnection &connection, Message msg) override;
	void disconnected(IConnection &connection) override;
	/* < IConnection::IHandler > */

	ClientGui clientGui;
	ConnectionToServer serverConnection;
	ClientController clientController;
};


Client::Impl::Impl(std::string ip, std::string port, std::string playerName,
                   std::string playerTeam, int windowWidth, int windowHeight)
				: clientGui{clientController, playerName, playerTeam, windowWidth,
				            windowHeight},
				  serverConnection{ip, port},
				  clientController{{playerName, playerTeam}, clientGui, serverConnection} {}

void Client::Impl::received(IConnection & /*connection*/, Message msg) {

	clientController.received(std::move(msg));
}

void Client::Impl::disconnected(IConnection &connection) {

	auto &conn = dynamic_cast<ConnectionToServer &>(connection);
	cout << "Server " << conn.socket().remote_endpoint() << " disconnected."
	     << endl;
	clientController.stop();
}

void Client::Impl::run() {

	serverConnection.listen(*this);

	clientController.start();

	ConnectionToServer::Statistics stats = serverConnection.getStatistics();
	cout << "Connection closed." << endl;
	cout << "Total bytes received: " << stats.bytes_received << endl;
	cout << "Total bytes transmitted: " << stats.bytes_sent << endl;
}


// Client Wrapper Definition
Client::Client(std::string ip, std::string port, std::string playerName,
               std::string playerTeam, int windowWidth, int windowHeight) :
				mImpl(make_unique<Impl>(move(ip), move(port),
				                        move(playerName), move(playerTeam),
				                        windowWidth, windowHeight)) {}
Client::~Client() = default;

void Client::run() { mImpl->run(); }