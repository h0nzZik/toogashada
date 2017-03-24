#include <stdexcept>
#include <iostream>
#include <string>  // std::string
#include <cstring> // std::memcpy
#include <boost/array.hpp>

#include "ConnectionToServer.h"

using namespace std;
using boost::asio::ip::tcp;

ConnectionToServer::ConnectionToServer(string host, string port) :
	socket{io_service}
{
	tcp::resolver resolver(io_service);
	tcp::resolver::query query(host, port /*"2061"*/);
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	boost::asio::connect(socket, endpoint_iterator);
}

void ConnectionToServer::start_rx_header() {
	cout << "start_rx_header" << endl;
	async_read(socket, boost::asio::buffer(rx_header_buffer), [this](
				const boost::system::error_code& error,
				std::size_t bytes_transferred){

			if (error)
			throw boost::system::system_error(error);

			if (bytes_transferred != sizeof(rx_header_buffer))
			throw std::runtime_error("Header problem");

			Header rx_header;
			memcpy(&rx_header, rx_header_buffer, sizeof(rx_header));
			cout << "Rx header: tag=" << rx_header.tag << ", len=" << rx_header.len << endl;
			rx_message = Message{};
			rx_message.tag = static_cast<Tag>(rx_header.tag);
			rx_message.data.resize(rx_header.len);

			async_read(socket, boost::asio::buffer(rx_message.data), [this](
						const boost::system::error_code& error,
						std::size_t bytes_transferred){
					
					if (error)
						throw boost::system::system_error(error);

					if (bytes_transferred != sizeof(rx_header_buffer))
						throw std::runtime_error("Data problem");

					cout << "Rx data." << endl;
					rx_handle(std::move(rx_message));
					start_rx_header();
			});
	});
}

void ConnectionToServer::send(Message message) {
	(void)message;
}

void ConnectionToServer::listen(std::function<void(Message)> handle) {
	rx_handle = handle;
	start_rx_header();
}

