//standard
#include <iostream>
#include <memory> // make_unique

// SDL
#include <SDL.h>

// project
#include "ClientGui.h"
#include "Message.h"
#include "Messages.h"
#include "Tag.h"

// self
#include "ClientController.h"

using namespace std;

ClientController::ClientController(ClientGui & clientGui) :
	clientGui(clientGui), quit{false}
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

		default:
			break;
	}
}
