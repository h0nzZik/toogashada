#include <iostream>
#include <chrono>
#include <thread>

#include <ctime>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "ConnectionToClient.hpp"

using namespace std;
using namespace std::chrono_literals;
using boost::asio::ip::tcp;

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

#if 0
class tcp_connection
: public boost::enable_shared_from_this<tcp_connection>
{
	public:
		using pointer = boost::shared_ptr<tcp_connection>;

		static pointer create(boost::asio::io_service& io_service)
		{
			return pointer(new tcp_connection(io_service));
		}

		tcp::socket& socket()
		{
			return _socket;
		}

		void start()
		{
			_remote = _socket.remote_endpoint();
			cout << "Connection from IP " << _remote.address() << ", port " << _remote.port() << endl;
			start_rx();
			start_tx();
		}

		~tcp_connection() {
			cout << "~tcp_connection()" << endl;
			//auto remote = _socket.remote_endpoint();
			//cout << "Connection closed " << remote.address() << ":" << remote.port() << endl;
		}

	private:
		tcp_connection(boost::asio::io_service& io_service)
			: _socket(io_service), _message{}, _recv_buffer{}
		{
		}

		void start_tx() {
			_message = make_daytime_string();

			boost::asio::async_write(_socket, boost::asio::buffer(_message),
					boost::bind(&tcp_connection::handle_write, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));

		}

		void start_rx() {
			_socket.async_receive(
					boost::asio::buffer(_recv_buffer),
					boost::bind(&tcp_connection::handle_receive, shared_from_this(),
						boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

		}

		void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred)
		{
			if ((boost::asio::error::eof == error) ||
					(boost::asio::error::connection_reset == error))
			{
				// handle the disconnect.
				cout << "Client disconnected " << _remote.address() << ":" << _remote.port() << endl;
				return;

			}

			cout << "Received " << bytes_transferred << " bytes." << endl;
			// read the data 
		}

		void handle_write(const boost::system::error_code& /*error*/,
				size_t /*bytes_transferred*/)
		{
			cout << "Write finished" << endl;
		}

		tcp::socket _socket;
		tcp::endpoint _remote;
		std::string _message;
		std::array<char, 64> _recv_buffer;
};
#endif

class tcp_server
{
	public:
		tcp_server(boost::asio::io_service& io_service, int port)
			: acceptor(io_service, tcp::endpoint(tcp::v4(), port))
		{
			start_accept();
		}

	private:
		void start_accept()
		{
			ConnectionToClient::pointer new_connection =
				ConnectionToClient::create(acceptor.get_io_service());

			acceptor.async_accept(
					new_connection->socket(),
					boost::bind(&tcp_server::handle_accept, this, new_connection,
						boost::asio::placeholders::error));
		}

		void handle_accept(ConnectionToClient::pointer new_connection,
				const boost::system::error_code& error)
		{
			if (!error)
			{
				cout << "Client " << new_connection->socket().remote_endpoint() << "connected." << endl;
				connections.push_back(new_connection);
				new_connection->listen([new_connection](Message /*m*/) {
					cout << "Server received message from " << new_connection->socket().remote_endpoint() << endl;

					Message m;
					m.tag = static_cast<Tag>(12); // why not?
					m.data = Message::Data{ 3, 4, 5};
					new_connection->send(move(m));
	
				});

				//std::this_thread::sleep_for(2s);

				// Send some hello message
				Message m;
				m.tag = static_cast<Tag>(25); // why not?
				m.data = Message::Data{1, 2, 3, 4, 5};
				new_connection->send(move(m));
			}

			start_accept();
		}

		tcp::acceptor acceptor;
		std::vector<ConnectionToClient::pointer> connections;
};

void server(int port)
{
	boost::asio::io_service io_service;
	tcp_server server(io_service, port);
	io_service.run(); // TODO tohle potrebujeme i v klientovi!
	cout << "Service stopped...?" << endl;
}

// Main class of the server
class GameServer {

};


