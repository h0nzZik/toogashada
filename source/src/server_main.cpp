#include <iostream>
#include <thread>
#include <csignal>

#include "Server.h"

using namespace std;

static Server * p_server = nullptr;

static void on_signal(int signal) {
	if (!p_server)
		return;

	cout << "Signal " << signal << " caught." << endl;
	p_server->shutdown();
}

void server(int port)
{
	Server server(port);
	p_server = &server;
	std::signal(SIGINT, on_signal);

	server.run();
	cout << "Server exited normally" << endl;
}

int main()
{
	cout << "Server\n";
	try {
		server(2048 + 13);
	} catch (std::exception& e)	{
		std::cerr << "main: " << e.what() << std::endl;
	}
}


