#pragma once

#include <string>

#include <common/Connection.h>

class ConnectionToServer : public Connection {
	public:
		ConnectionToServer(std::string host, std::string port);
		~ConnectionToServer() = default;

		void run();

		boost::asio::ip::tcp::socket & socket() { return _socket; }

	private:
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::socket _socket;
};
