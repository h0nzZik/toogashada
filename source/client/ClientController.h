#ifndef SRC_CLIENTCONTROLLER_H_
#define SRC_CLIENTCONTROLLER_H_

#include <mutex>
#include <common/GameObjectManager.h>
#include "ClientPlayer.h"

class ClientGui;
class RemoteServerWrapper;

struct Message;
struct MsgNewPolygonalObject;
struct MsgObjectPosition;
struct ServerMessage;
struct ClientMessage;

class ClientController final {
public:
	explicit ClientController(ClientPlayer &player, ClientGui & clientGui, RemoteServerWrapper & server);
	~ClientController() = default;

	void received(Message msg);
	void main_loop();

private:
	class Receiver;
	void redraw();
	void received(MsgNewPolygonalObject msg);
	void received(MsgObjectPosition msg);
	void received(ServerMessage msg);
	void handle_event(SDL_Event const & e);
	void handleKeyPress(SDL_Scancode code);
	void handleKeyRelease(SDL_Scancode code);
	void send(ClientMessage const & msg);

	ClientGui & clientGui;
	RemoteServerWrapper & remoteServer;
	ClientPlayer &player;

	bool quit;

	GameObjectManager gameObjects;
	std::mutex mutexGameObjects;


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
