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
  explicit ClientController(PlayerInfo &player, ClientGui &clientGui,
                            ConnectionToServer &server);

  void received(Message msg);
  void main_loop();
  void loopWork();
  void stop();
  bool isMyPlayer(const EntityID &id);
  entity_t getMyPlayer();

  entity_t getEntity(const EntityID &id);

private:
  std::map<SDL_Scancode, std::pair<PlayerAction, bool>> keyMap{
      {SDL_SCANCODE_W, {PlayerAction::Up, 0}},
      {SDL_SCANCODE_S, {PlayerAction::Down, 0}},
      {SDL_SCANCODE_A, {PlayerAction::Left, 0}},
      {SDL_SCANCODE_D, {PlayerAction::Right, 0}},
      {SDL_SCANCODE_SPACE, {PlayerAction::Fire, 0}}};

  class Receiver;
  void redraw();
  void received(ServerMessage msg);
  void dispatchKeyAndMouseStates();
  void dispatchKeyStates();
  void send(ClientMessage const &msg);

  ClientGui &clientGui;
  ConnectionToServer &remoteServer;
  PlayerInfo &player;
  EntityID playerId;
  entity_t playerEntity;
  GameInfo gameInfo;

  bool continueLoop;

  EntityComponentSystem ecs;
  std::map<EntityID, entity_t> entites;
};
