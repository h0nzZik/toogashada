#include <iostream>
#include <cstring> // std::memcpy
#include <boost/array.hpp>

#include "Connection.h"

using namespace std;
using boost::asio::ip::tcp;

Connection::Connection(boost::asio::ip::tcp::socket & socket) :
	socket{socket}
{
	;
}

Connection::~Connection() {

}

void Connection::start_rx_header() {
	async_read(socket, boost::asio::buffer(rx_header_buffer), /*boost::asio::transfer_all(), */[this](
				const boost::system::error_code& error,
				std::size_t bytes_transferred){

			cout << "async_read callback" << endl;
			if (error) {
				cout << "Reading message header failed" << endl;
				throw boost::system::system_error(error);
			}

			if (bytes_transferred != sizeof(rx_header_buffer))
				throw std::runtime_error("Header problem");

			Header rx_header;
			memcpy(&rx_header, rx_header_buffer, sizeof(rx_header));
			cout << "Rx header: tag=" << rx_header.tag << ", len=" << rx_header.len << endl;
			rx_message = Message{};
			rx_message.tag = static_cast<Tag>(rx_header.tag);
			rx_message.data.resize(rx_header.len);

			async_read(socket, boost::asio::buffer(rx_message.data), boost::asio::transfer_all(), [this](
						const boost::system::error_code& error,
						std::size_t bytes_transferred){
	
					cout << "Received message from " << socket.remote_endpoint() << endl;
					cout << "Bytes: " << bytes_transferred << endl;
					if (error) {
						cout << "Connection::rx error: " << error << endl;
						throw boost::system::system_error(error);
					}

					if (bytes_transferred != rx_message.data.size())
						throw std::runtime_error("Data problem");

					rx_handle(std::move(rx_message));
					start_rx_header();
			});
	});
}

void Connection::send(Message message) {
	(void)message;
	Header tx_header;
	tx_header.tag = static_cast<uint32_t>(message.tag);
	tx_header.len = message.data.size();
	uint8_t tx_header_buf[sizeof(Header)];
	memcpy(tx_header_buf, &tx_header, sizeof(Header));
	try {
		size_t cnt = write(socket, boost::asio::buffer(tx_header_buf));
		cnt += write(socket, boost::asio::buffer(message.data));
		cout << "Write done (" << cnt << " bytes)" << endl;
	} catch (exception & e) {
		cerr << "Connection::send() failed: " << e.what() << endl;
	}
}

void Connection::listen(std::function<void(Message)> handle) {
	cout << "Listening on " << socket.local_endpoint() << " to " << socket.remote_endpoint() << endl;
	rx_handle = handle;
	start_rx_header();
}

