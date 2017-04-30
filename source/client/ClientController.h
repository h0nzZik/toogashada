#pragma once

#include <memory>
#include <SDL_scancode.h>

struct ClientPlayer;
class ClientGui;
class RemoteServerWrapper;
struct Message;

class ClientController final {
public:
	explicit ClientController(ClientPlayer &player, ClientGui & clientGui, RemoteServerWrapper & server);
	~ClientController();

	void received(Message msg);
	void main_loop();
	void stop();

private:
	class Impl;
	std::unique_ptr<Impl> impl;

	struct Config {
		Config();

		SDL_Scancode key_up;
		SDL_Scancode key_down;
		SDL_Scancode key_left;
		SDL_Scancode key_right;
		SDL_Scancode key_shoot;
	};
};

