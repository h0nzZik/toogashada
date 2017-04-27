//standard
#include <iostream>
#include <memory> // make_unique

// SDL
#include <SDL.h>

#include <boost/variant/static_visitor.hpp>

// project
#include <common/IConnection.h>
#include <common/Message.h>
#include <common/Messages.h>
#include <common/Tag.h>
#include <common/AnyComponent.h>
#include <common/ServerMessage.h>
#include <common/ClientMessage.h>

#include "ClientGui.h"
#include "RemoteServerWrapper.h"

// self
#include "ClientController.h"

using namespace std;

ClientController::ClientController(ClientPlayer &player,
								   ClientGui & clientGui,
								   RemoteServerWrapper & server) :
	clientGui(clientGui),
	remoteServer{server},
    player(player),
    quit{false}
{}

class ClientController::Receiver : public boost::static_visitor<void> {
	public:
		Receiver(ClientController & controller) : controller(controller) {}

		void operator()(MsgNewPolygonalObject const & msg) {
			// fallback
			controller.received(msg);
		}

		void operator()(MsgObjectPosition const & msg) {
			// fallback
			controller.received(msg);
		}

		void operator()(MsgNewPlayer const & msg) {
			;
		}

		void operator()(MsgNewEntity const & msg) {
			;
		}

		void operator()(MsgUpdateEntity const & msg) {
			;
		}


		// todo template rest

	private:
		ClientController & controller;
};

void ClientController::received(Message msg) {

	switch(msg.tag) {
		case Tag::Hello:
			cout << "Server says hello." << endl;
			break;

		case Tag::UniversalServerMessage:
			return received(ServerMessage::from(msg));

		default:
			break;
	}
}

void ClientController::received(ServerMessage msg) {
	Receiver receiver{*this};
	boost::apply_visitor(receiver, msg.data);
}

void ClientController::received(MsgNewPolygonalObject msg) {
	cout << "PolygonalObject with id " << msg.object_id << endl;
	auto obj = make_unique<GameObject>(msg.object_id);
	obj->center = msg.center;
	obj->shape = move(msg.shape);

	std::lock_guard<std::mutex> guard{mutexGameObjects};
	gameObjects.insert(move(obj));
}

void ClientController::received(MsgObjectPosition msg) {
	gameObjects.getObjectById(msg.object_id).center = msg.new_center;
}

void ClientController::main_loop() {
	while( !quit )
	{
		SDL_Event e;
		while( SDL_PollEvent( &e ) != 0 )
			handle_event(e);

		redraw();
	}
	cout << "Quitting gui\n";
}

void ClientController::redraw() {

	{
		std::lock_guard<std::mutex> guard{mutexGameObjects};
		// TODO(h0nzZik): I think it is not needed to lock the whole structure.
		// We should refine it someday in future.

        clientGui.renderGui(gameObjects);

//        for (GameObject const & go : gameObjects) {
//
//            clientGui.render_polygon(go.center, go.shape);
//        }

	}

}


/**
 * A note about how we compute object positions.
 * Server periodically sends the authoritative information about object positions.
 * However
 */

void ClientController::handle_event(SDL_Event const & e) {
	switch(e.type) {
		case SDL_QUIT:
			quit = true;
			break;

		case SDL_WINDOWEVENT:
			//cout << "Window event" << endl;
			break;

		case SDL_MOUSEMOTION:
			// cout << "Mouse at [" << e.motion.x << "," << e.motion.y << "]" << endl;
			break;

		case SDL_KEYDOWN:
			if (e.key.repeat == 0)
				handleKeyPress(e.key.keysym.scancode);
			break;

		case SDL_KEYUP:
			handleKeyRelease(e.key.keysym.scancode);
			break;

		default:
			break;
	}
}

void ClientController::send(ClientMessage const & msg) {
	remoteServer.conn().send(msg.to_message());
}

void ClientController::handleKeyPress(SDL_Scancode code) {
	ClientMessage msg;

	if (code == config.key_down)
	{
		msg = {MsgPlayerMovesBackward{}};
		pressedKeys.key_down = true;
	}
	else if (code == config.key_left)
	{
		msg = {MsgPlayerMovesLeft{}};
		pressedKeys.key_left = true;
	}
	else if (code == config.key_right)
	{
		msg = {MsgPlayerMovesRight{}};
		pressedKeys.key_right = true;
	}
	else if (code == config.key_up)
	{
		msg = {MsgPlayerMovesForward{}};
		pressedKeys.key_up = true;
	}
	else
		return;

	send(msg);
}

void ClientController::handleKeyRelease(SDL_Scancode code) {
	cout << "keyup: " << code << endl;

	if (code == config.key_down)
	{
		pressedKeys.key_down = false;
	}
	else if (code == config.key_left)
	{
		pressedKeys.key_left = false;
	}
	else if (code == config.key_right)
	{
		pressedKeys.key_right = false;
	}
	else if (code == config.key_up)
	{
		pressedKeys.key_up = false;
	}
	else
		return;

	if (!pressedKeys.key_down && !pressedKeys.key_up &&
			!pressedKeys.key_left && !pressedKeys.key_right)
		remoteServer.conn().send(Message(Tag::PlayerStops));
}

ClientController::Config::Config()
{
	key_up = SDL_SCANCODE_W;
	key_down = SDL_SCANCODE_S;
	key_left = SDL_SCANCODE_A;
	key_right = SDL_SCANCODE_D;
}
