#pragma once

#include <SDL_scancode.h>
#include <memory>

#include <iostream>
#include <map>

// SDL
#include <SDL.h>

// Boost
#include <boost/variant/static_visitor.hpp>

// project
#include <common/AnyComponent.h>
#include <common/ClientMessage.h>
#include <common/IConnection.h>
#include <common/Message.h>
#include <common/Messages.h>
#include <common/ServerMessage.h>
#include <common/Tag.h>
#include <common/components/EntityID.h>
#include <common/components/PlayerInfo.h>

#include "ClientGui.h"

struct PlayerInfo;
class ClientGui;
class ConnectionToServer;
struct Message;

class ClientController final {
public:
  explicit ClientController(PlayerInfo &&player, ClientGui &clientGui,
                            ConnectionToServer &server);

  void start();
	void stop();
	void received(Message msg);


  bool isMyPlayer(const EntityID &id) const;
  entity_t getMyPlayer();

private:

	ClientGui &mClientGui;
	ConnectionToServer &mRemoteServer;
	PlayerInfo mPlayerInfo;
	EntityID mPlayerId;
	entity_t mPlayerEntity;
	GameInfo mGameInfo;

	bool mEndLoop = false;

	EntityComponentSystem mEcs;
	std::map<EntityID, entity_t> mEntites;


  std::map<SDL_Scancode, std::pair<PlayerAction, bool>> keyMap{
      {SDL_SCANCODE_W, {PlayerAction::Up, 0}},
      {SDL_SCANCODE_S, {PlayerAction::Down, 0}},
      {SDL_SCANCODE_A, {PlayerAction::Left, 0}},
      {SDL_SCANCODE_D, {PlayerAction::Right, 0}},
      {SDL_SCANCODE_SPACE, {PlayerAction::Fire, 0}}};


  class Receiver;
	void received(ServerMessage msg);

	void send(ClientMessage const &msg);
  void sendUserInteractionToServer();

	void loopWork();

	entity_t getEntity(const EntityID &id);
};