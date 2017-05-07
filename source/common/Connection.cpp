#include <boost/array.hpp>
#include <cstring> // std::memcpy
#include <iostream>

#include "Connection.h"
#include "Tag.h"

using namespace std;
using boost::asio::ip::tcp;

Connection::Connection(boost::asio::ip::tcp::socket &socket) : socket{socket} {
  ;
}

Connection::~Connection() {}

bool Connection::whatIfDisconnected(boost::system::error_code const &error) {
  if ((boost::asio::error::eof == error) ||
      (boost::asio::error::connection_reset == error)) {
    handler->disconnected(*this);
    return true;
  }

  return false;
}

void Connection::start_rx_header() {
  async_read(socket, boost::asio::buffer(rx_header_buffer),
             /*boost::asio::transfer_all(), */ [this](
                 const boost::system::error_code &error,
                 std::size_t bytes_transferred) {

               if (whatIfDisconnected(error))
                 return;

               if (error) {
                 cout << "Reading message header failed" << endl;
                 throw boost::system::system_error(error);
               }

               m_statistics.bytes_received += bytes_transferred;

               if (bytes_transferred != sizeof(rx_header_buffer))
                 throw std::runtime_error("Header problem");

               Header rx_header;
               memcpy(&rx_header, rx_header_buffer, sizeof(rx_header));
               rx_message = Message{};
               rx_message.tag = static_cast<Tag>(rx_header.tag);
               rx_message.data.resize(rx_header.len);

               async_read(socket, boost::asio::buffer(rx_message.data),
                          boost::asio::transfer_all(),
                          [this](const boost::system::error_code &error,
                                 std::size_t bytes_transferred) {

                            if (whatIfDisconnected(error))
                              return;

                            if (error) {
                              cout << "Connection::rx error: " << error << endl;
                              throw boost::system::system_error(error);
                            }

                            m_statistics.bytes_received += bytes_transferred;

                            if (bytes_transferred != rx_message.data.size())
                              throw std::runtime_error("Data problem");

                            received(std::move(rx_message));
                            start_rx_header();
                          });
             });
}

void Connection::received(Message message) {
  // cout << "Received message from " << socket.remote_endpoint() << endl;
  // cout << "  tag=" << int(message.tag) << ", len=" << message.data.size() <<
  // endl;

  switch (message.tag) {
  case Tag::Ping:
    cout << "Got ping" << endl;
    message.tag = Tag::Pong;
    send(std::move(message));
    return;

  default:
    handler->received(*this, std::move(message));
    break;
  }
}

void Connection::send(Message message) {
  try {
    (void)message;
    Header tx_header;
    tx_header.tag = static_cast<uint32_t>(message.tag);
    tx_header.len = message.data.size();
    uint8_t tx_header_buf[sizeof(Header)];
    memcpy(tx_header_buf, &tx_header, sizeof(Header));
    write(socket, boost::asio::buffer(tx_header_buf));
    write(socket, boost::asio::buffer(message.data));
    m_statistics.bytes_sent += sizeof(Header) + message.data.size();
  } catch (exception &e) {
    cout << "Connection::send: " << e.what() << endl;
    handler->disconnected(*this);
  }
}

void Connection::listen(IHandler &handler) {
  cout << "Listening on " << socket.local_endpoint() << " to "
       << socket.remote_endpoint() << endl;
  this->handler = &handler;
  start_rx_header();
}
