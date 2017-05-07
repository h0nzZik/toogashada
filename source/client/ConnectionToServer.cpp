#include <iostream>
#include <stdexcept>
#include <string> // std::string

#include "ConnectionToServer.h"

using namespace std;
using boost::asio::ip::tcp;

ConnectionToServer::ConnectionToServer(string host, string port)
    : Connection{_socket}, _socket{io_service} {
  std::cout << "Connecting to " << host << ":" << port << std::endl;
  tcp::resolver resolver(io_service);
  tcp::resolver::query query(host, port);
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  boost::asio::connect(_socket, endpoint_iterator);
}

void ConnectionToServer::run() {
	io_service.run();
	Statistics stats = getStatistics();
	cout << "Connection closed." << endl;
	cout << "Total bytes received: " << stats.bytes_received << endl;
	cout << "Total bytes transmitted: " << stats.bytes_sent << endl;
}

void ConnectionToServer::stop() {
	// TODO maybe send
	io_service.stop();
}
