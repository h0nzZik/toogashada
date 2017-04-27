#include <stdexcept>
#include <string>  // std::string
#include <iostream>

#include "ConnectionToServer.h"

using namespace std;
using boost::asio::ip::tcp;

ConnectionToServer::ConnectionToServer(string host, string port) :
	Connection{_socket},
	_socket{io_service}
{
	std::cout << "Connecting to " << host << ":" << port << std::endl;

	tcp::resolver resolver(io_service);
	tcp::resolver::query query(host, port);
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	boost::asio::connect(_socket, endpoint_iterator);
}

void ConnectionToServer::run()
{
	io_service.run();
}

