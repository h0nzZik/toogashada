#pragma once

#include <common/Connection.h>

class ConnectionToClient : public Connection {
public:
  explicit ConnectionToClient(boost::asio::io_service &io_service);
  ~ConnectionToClient();

  boost::asio::ip::tcp::socket &socket() { return _socket; }

private:
  boost::asio::io_service &_io_service;
  boost::asio::ip::tcp::socket _socket;
};
