#include <stdexcept>
#include <string>  // std::string

#include "ConnectionToServer.h"

using namespace std;
using boost::asio::ip::tcp;

ConnectionToServer::ConnectionToServer(string host, string port) :
	Connection{_socket},
	_socket{io_service}
{
	tcp::resolver resolver(io_service);
	tcp::resolver::query query(host, port /*"2061"*/);
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	boost::asio::connect(_socket, endpoint_iterator);
}

void ConnectionToServer::run()
{
	io_service.run();
}

