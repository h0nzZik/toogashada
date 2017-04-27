#ifndef SRC_CLIENT_H_
#define SRC_CLIENT_H_

#include "ConnectionToServer.h"
#include "ClientGui.h"
#include "ClientController.h"
#include "RemoteServerWrapper.h"

class Client : private IConnection::IHandler {
public:

	Client(std::string ip, std::string port, std::string playerName, std::string playerTeam);

	~Client() = default;

	void run();
private:
	/* < IConnection::IHandler > */
	void received(IConnection & connection, Message msg) override;
	void disconnected(IConnection & connection) override;
	/* < IConnection::IHandler > */

	ClientGui clientGui;
	ConnectionToServer serverConnection;
	RemoteServerWrapper remoteServerWrapper;
	ClientController clientController;
    ClientPlayer clientPlayer;
};

#endif /* SRC_CLIENT_H_ */
