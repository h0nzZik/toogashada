#ifndef SRC_CLIENTCONTROLLER_H_
#define SRC_CLIENTCONTROLLER_H_

#include <mutex>
#include "GameObjectManager.h"

class ClientGui;
struct Message;
struct MsgNewPolygonalObject;

class ClientController final {
public:
	explicit ClientController(ClientGui & clientGui);
	~ClientController() = default;

	void received(Message msg);
	void main_loop();

private:
	void redraw();
	void received(MsgNewPolygonalObject msg);
	void handle_event(SDL_Event const & e);

	ClientGui & clientGui;

	bool quit;
	GameObjectManager gameObjects;
	std::mutex mutexGameObjects;
};

#endif /* SRC_CLIENTCONTROLLER_H_ */
