#ifndef SRC_CLIENTCONTROLLER_H_
#define SRC_CLIENTCONTROLLER_H_

#include <mutex>
#include "GameObjectManager.h"
#include "ClientPlayer.h"

class ClientGui;
class RemoteServerWrapper;

struct Message;
struct MsgNewPolygonalObject;

class ClientController final {
public:
	explicit ClientController(ClientGui & clientGui, RemoteServerWrapper & server);
	~ClientController() = default;

	void received(Message msg);
	void main_loop();

private:
	void redraw();
	void received(MsgNewPolygonalObject msg);
	void handle_event(SDL_Event const & e);
	void handleKeyPress(SDL_Scancode code);
	void handleKeyRelease(SDL_Scancode code);

	ClientGui & clientGui;
	RemoteServerWrapper & remoteServer;

	bool quit;
	GameObjectManager gameObjects;
	std::mutex mutexGameObjects;
	ClientPlayer clientPlayer;

	struct Config {
		Config();

		SDL_Scancode key_up;
		SDL_Scancode key_down;
		SDL_Scancode key_left;
		SDL_Scancode key_right;
	};

	struct PressedKeys {
		bool key_up = false;
		bool key_down = false;
		bool key_left = false;
		bool key_right = false;
	};

	PressedKeys pressedKeys;

	Config config;
};

#endif /* SRC_CLIENTCONTROLLER_H_ */
