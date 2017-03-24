#pragma once

#include <string>
#include <boost/asio.hpp>
#include "IConnection.h"

// TODO factor out the messaging stuff

class ConnectionToServer : public IConnection {
	public:
		ConnectionToServer(std::string host, std::string port);
		~ConnectionToServer() = default;

		// < IConnection >
		void send(Message message) override;
		void listen(std::function<void(Message)> handle) override;
		// </IConnection >

	private:
		void start_rx_header();
		void start_rx_data();

		boost::asio::io_service io_service;
		boost::asio::ip::tcp::socket socket;
		uint8_t rx_header_buffer[8];

		struct Header {
			uint32_t tag;
			uint32_t len;
		} __attribute__((packed));

		static_assert(sizeof(Header) == sizeof(rx_header_buffer), "Packed");

		Message rx_message;
		std::function<void(Message)> rx_handle;
};
