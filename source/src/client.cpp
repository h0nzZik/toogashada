#include <stdexcept>
#include <memory>
#include <iostream>
#include <SDL2/SDL.h>
#include <boost/asio.hpp>
#include <string>

#include "Message.h"
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

	auto screenSurface = SDL_GetWindowSurface( window );
	SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x20, 0x80, 0xFF ) );
	SDL_UpdateWindowSurface( window );
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
			handle_user_event(e.user);
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
	if (e.code != Sint32(rcvd_message_event))
		return;

	auto message = unique_ptr<Message>(static_cast<Message *>(e.data1));
}

class Client {
	public:
		Client();
		~Client() = default;

		void run();
	private:
		ClientGui gui;
		ConnectionToServer conn;
};

Client::Client() :
	conn{"localhost", "2061"}
{
	;
}

void Client::run() {
	conn.listen([this](Message msg) {
		gui.on_message(std::move(msg));
	});
	// TODO send 'hello' message
	gui.run();
}

int main()
{
	cout << "Client\n";
	Client client;
	client.run();
}
