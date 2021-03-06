#include <iostream>

#include "ConnectionToClient.hpp"

using namespace std;

ConnectionToClient::ConnectionToClient(boost::asio::io_service &io_service)
    : Connection{_socket}, _io_service{io_service}, _socket{_io_service} {
  ;
}

ConnectionToClient::~ConnectionToClient() { ; }
