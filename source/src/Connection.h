#pragma once

#include <boost/asio.hpp>

#include "IConnection.h"

class Connection : public IConnection {
	public:
		explicit Connection(boost::asio::ip::tcp::socket & socket);
		~Connection();

		// < IConnection >
		void send(Message message) override;
		void listen(std::function<void(Message)> handle) override;
		// </IConnection >

	private:
		void start_rx_header();
		void start_rx_data();

		boost::asio::ip::tcp::socket & socket;

		uint8_t rx_header_buffer[8];

		struct Header {
			uint32_t tag;
			uint32_t len;
		} __attribute__((packed));

		static_assert(sizeof(Header) == sizeof(rx_header_buffer), "Packed");

		Message rx_message;
		std::function<void(Message)> rx_handle;
};
