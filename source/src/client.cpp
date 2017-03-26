// Standard
#include <stdexcept>
#include <memory>
#include <iostream>
#include <string>
#include <thread>

// Boost
#include <boost/asio.hpp>

// SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

// Project
#include "Tag.h"
#include "Message.h"
#include "Messages.h"
#include "ConnectionToServer.h"

using namespace std;

void client_networking(string host);


class ClientGui {
	public:
		ClientGui();
		~ClientGui();

		void run();
		void on_message(Message msg);

	private:
		void handle_event(SDL_Event const & e);
		void handle_user_event(SDL_UserEvent const &e);
		bool quit = false;
		SDL_Window * window;
		SDL_Renderer * renderer;
		uint32_t rcvd_message_event;
};

ClientGui::ClientGui()
{
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
		throw std::runtime_error("Cannot initialize SDL: " + string(SDL_GetError()));

	//Screen dimension constants
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if (!window)
		throw std::runtime_error("Cannot create window: " + string(SDL_GetError()));

	rcvd_message_event = SDL_RegisterEvents(1);
	if (rcvd_message_event == static_cast<uint32_t>(-1))
		throw std::runtime_error("Cannot register events");


	//auto screenSurface = SDL_GetWindowSurface( window );
	//SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x20, 0x80, 0xFF ) );
	//SDL_UpdateWindowSurface( window );

	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	if(!renderer)
		throw std::runtime_error("Cannot create renderer: " + string(SDL_GetError()));
	SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
}

ClientGui::~ClientGui() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void ClientGui::run() {

	// TODO: add a custom event, which represents data from network.
	//  Every real processing will be done in this loop.

	//While application is running
	while( !quit )
	{
		//Event handler
		SDL_Event e;
		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			handle_event(e);
		}

		//Apply the image
		//SDL_BlitSurface( gXOut, NULL, gScreenSurface, NULL );

		//Update the surface
		SDL_UpdateWindowSurface( window );
	}
	cout << "Quitting\n";
}

void ClientGui::handle_event(SDL_Event const & e) {
	switch(e.type) {
		case SDL_QUIT:
			quit = true;
			break;

		case SDL_WINDOWEVENT:
			cout << "Window event" << endl;
			break;
	
		case SDL_MOUSEMOTION:
			cout << "Mouse at [" << e.motion.x << "," << e.motion.y << "]" << endl;
			break;

		case SDL_USEREVENT:
			cout << "User event" << endl;
			handle_user_event(e.user);
			break;

		default:
			break;
	}
}

void ClientGui::on_message(Message msg) {
	auto m = make_unique<Message>(std::move(msg));

	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.type = rcvd_message_event;
	event.user.data1 = m.release();
	event.user.data2 = nullptr;
	SDL_PushEvent(&event);
}

void ClientGui::handle_user_event(SDL_UserEvent const &e) {
	cout << "User event code: " << e.type << endl;
	if (e.type != rcvd_message_event) {
		cout << "User event: unknown code" << endl;
		return;
	}

	cout << "User event with correct code" << endl;

	auto message = unique_ptr<Message>(static_cast<Message *>(e.data1));
	cout << "From server: " << *message << "\n";
	switch(message->tag) {
		case Tag::Hello:
			cout << "Server says hello." << endl;
			break;

		case Tag::NewPolygonalObject:
			{
				// TODO draw it
				auto obj = MsgNewPolygonalObject::from(*message);
				//boxRGBA(surface, 
				cout << "PolygonalObject" << endl;
				//boxRGBA(renderer, 10, 10, 50, 30, 255, 200, 0, 128);

				size_t const n = obj.points.size();
				auto xs = make_unique<Sint16[]>(n);
				auto ys = make_unique<Sint16[]>(n);
				for (size_t i = 0; i < n; i++) {
					xs[i] = obj.points[i].x;
					ys[i] = obj.points[i].y;
				}
				polygonRGBA(renderer, xs.get(), ys.get(), n, 255, 200, 150, 128);
				/*
				int polygonRGBA(SDL_Surface* dst, Sint16* vx, Sint16* vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a); */
				SDL_RenderPresent( renderer );



			}
			break;

		default:
			break;
	}
}

class Client : private IConnection::IHandler {
	public:
		Client();
		~Client() = default;

		void run();
	private:
		/* < IConnection::IHandler > */
		void received(IConnection & connection, Message msg) override;
		void disconnected(IConnection & connection) override;
		/* < IConnection::IHandler > */

		ClientGui gui;
		ConnectionToServer conn;
};

void Client::received(IConnection & connection, Message msg) {
	auto & conn = dynamic_cast<ConnectionToServer &>(connection);
	cout << "Client received message from " << conn.socket().remote_endpoint() << endl;	
	gui.on_message(std::move(msg));
}

void Client::disconnected(IConnection & connection) {
	auto & conn = dynamic_cast<ConnectionToServer &>(connection);
	cout << "Client " << conn.socket().remote_endpoint() << " disconnected." << endl;
}

Client::Client() :
	conn{"localhost", "2061"}
{
	;
}

void Client::run() {
	conn.listen(*this);
	conn.send(Message{Tag::Hello, {4, 5, 6}});
	
	thread network([&]{
			conn.run();
	});
	gui.run();
}

int main()
{
	cout << "Client\n";
	Client client;
	client.run();
}

