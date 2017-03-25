#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Connection.h"

class ConnectionToClient : public Connection,
	public boost::enable_shared_from_this<ConnectionToClient>
{
	public:
		using pointer = boost::shared_ptr<ConnectionToClient>;

		static pointer create(boost::asio::io_service& io_service);

		boost::asio::ip::tcp::socket& socket()
		{
			return _socket;
		}


		~ConnectionToClient();


	private:
		explicit ConnectionToClient(boost::asio::io_service & io_service);

		boost::asio::io_service & _io_service;
		boost::asio::ip::tcp::socket _socket;
};
