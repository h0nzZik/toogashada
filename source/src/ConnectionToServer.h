#pragma once

#include <string>

#include "Connection.h"

class ConnectionToServer : public Connection {
	public:
		ConnectionToServer(std::string host, std::string port);
		~ConnectionToServer() = default;

		void run();

	private:
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::socket socket;
};
