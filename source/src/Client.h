#ifndef SRC_CLIENT_H_
#define SRC_CLIENT_H_

#include "ConnectionToServer.h"
#include "ClientGui.h"
#include "ClientController.h"

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

	ClientGui clientGui;
	ConnectionToServer serverConnection;
	ClientController clientController;
};

#endif /* SRC_CLIENT_H_ */
