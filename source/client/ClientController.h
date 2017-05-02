#pragma once

#include <memory>
#include <SDL_scancode.h>

#include <iostream>
#include <mutex>
#include <map>
#include <thread>

// SDL
#include <SDL.h>

// Boost
#include <boost/variant/static_visitor.hpp>

// project
#include <common/IConnection.h>
#include <common/Message.h>
#include <common/Messages.h>
#include <common/Tag.h>
#include <common/AnyComponent.h>
#include <common/ServerMessage.h>
#include <common/ClientMessage.h>
#include "common/components/EntityID.h"

#include "ClientPlayer.h"
#include "ClientGui.h"
#include "RemoteServerWrapper.h"

struct ClientPlayer;
class ClientGui;
class RemoteServerWrapper;
struct Message;

class ClientController final {
public:
	explicit ClientController(ClientPlayer &player, ClientGui & clientGui, RemoteServerWrapper & server);

	void received(Message msg);
	void main_loop();
	void loopWork();
	void stop();
	bool isMyPlayer(const EntityID& id);

private:

	std::map<SDL_Scancode, std::pair<PlayerAction,bool>> keyMap {
			{SDL_SCANCODE_W,     {PlayerAction::Up,    0}},
			{SDL_SCANCODE_S,     {PlayerAction::Down,  0}},
			{SDL_SCANCODE_A,     {PlayerAction::Left,  0}},
			{SDL_SCANCODE_D,     {PlayerAction::Right, 0}},
			{SDL_SCANCODE_SPACE, {PlayerAction::Fire,  0}}
	};

	class Receiver;

	void redraw();
	void received(ServerMessage msg);
	void dispatchKeyStates();
	void send(ClientMessage const & msg);


	ClientGui & clientGui;
	RemoteServerWrapper & remoteServer;
	ClientPlayer &player;
	EntityID playerId;

	bool continueLoop;

	//GameObjectManager gameObjects;
	std::mutex mutexGameObjects;


	EntityComponentSystem ecs;
	std::map<EntityID, entity_t> entites;
};

