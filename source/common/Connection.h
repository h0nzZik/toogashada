#pragma once

#include <boost/asio.hpp>

#include "IConnection.h"

class Connection : public IConnection {
public:
  explicit Connection(boost::asio::ip::tcp::socket &socket);
  ~Connection() = default;

  Connection(Connection const & /*other*/) = delete;
  Connection &operator=(Connection const & /*other*/) = delete;

  // < IConnection >
  void send(Message message) override;
  void listen(IHandler &handler) override;
  // </IConnection >

  struct Statistics {
    size_t bytes_sent = 0;
    size_t bytes_received = 0;
  };

  Statistics getStatistics() const { return m_statistics; }

private:
  void start_rx_header();
  void start_rx_data();
  void received(Message m);
  bool whatIfDisconnected(boost::system::error_code const &error);

  boost::asio::ip::tcp::socket &socket;

  Statistics m_statistics;

  uint8_t rx_header_buffer[8];

  struct Header {
    uint32_t tag;
    uint32_t len;
  } __attribute__((packed));

  static_assert(sizeof(Header) == sizeof(rx_header_buffer), "Packed");

  Message rx_message;
  IHandler *handler = nullptr;
};
