//standard
#include <iostream>
#include <memory> // make_unique

// SDL
#include <SDL.h>

// project
#include <common/IConnection.h>
#include <common/Message.h>
#include <common/Messages.h>
#include <common/Tag.h>

#include "ClientGui.h"
#include "RemoteServerWrapper.h"

// self
#include "ClientController.h"

using namespace std;

ClientController::ClientController(ClientGui & clientGui, RemoteServerWrapper & server) :
	clientGui(clientGui), remoteServer{server}, quit{false}
{

}

void ClientController::received(Message msg) {

	switch(msg.tag) {
		case Tag::Hello:
			cout << "Server says hello." << endl;
			break;

		case Tag::NewPolygonalObject:
			return received(MsgNewPolygonalObject::from(msg));

		default:
			break;
	}
}

void ClientController::received(MsgNewPolygonalObject msg) {
	cout << "PolygonalObject with id " << msg.object_id << endl;
	auto obj = make_unique<GameObject>(msg.object_id);
	obj->center = msg.center;
	obj->shape = move(msg.shape);

	std::lock_guard<std::mutex> guard{mutexGameObjects};
	gameObjects.insert(move(obj));
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
	clientGui.clear_gui();
	{
		std::lock_guard<std::mutex> guard{mutexGameObjects};
		// TODO(h0nzZik): I think it is not needed to lock the whole structure.
		// We should refine it someday in future.

		for (GameObject const & go : gameObjects) {
			clientGui.render_polygon(go.center, go.shape);
		}
	}
	clientGui.update_gui();
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
			cout << "Window event" << endl;
			break;

		case SDL_MOUSEMOTION:
			cout << "Mouse at [" << e.motion.x << "," << e.motion.y << "]" << endl;
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

void ClientController::handleKeyPress(SDL_Scancode code) {
	cout << "keydown: " << code << endl;
	Message msg;

	if (code == config.key_down)
	{
		msg.tag = Tag::PlayerMovesBackward;
		pressedKeys.key_down = true;
	}
	else if (code == config.key_left)
	{
		msg.tag = Tag::PlayerMovesLeft;
		pressedKeys.key_left = true;
	}
	else if (code == config.key_right)
	{
		msg.tag = Tag::PlayerMovesRight;
		pressedKeys.key_right = true;
	}
	else if (code == config.key_up)
	{
		msg.tag = Tag::PlayerMovesForward;
		pressedKeys.key_up = true;
	}
	else
		return;

	remoteServer.conn().send(std::move(msg));
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
