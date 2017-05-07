// standard
#include <chrono>
#include <iostream>
#include <map>
#include <math.h>
#include <memory> // make_unique
#include <thread>

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
#include <common/components/PlayerInfo.h>

#include "ClientGui.h"
#include "ConnectionToServer.h"

// self
#include "ClientController.h"

using namespace std;
using namespace std::chrono_literals;


class ClientController::Receiver : public boost::static_visitor<void> {

public:
	Receiver(ClientController &controller) : controller(controller) {}

	void operator()(MsgNewEntity const &msg) {

		controller.mEntites[msg.entity_id] =
						controller.mEcs.entityManager.create_entity(msg.entity_id);
		entity_t entity = controller.getEntity(msg.entity_id);
		EntityComponentSystem::add_components(entity, msg.components);
	}

	void operator()(MsgUpdateComponents const &msg) {

		entity_t entity = controller.getEntity(msg.entity_id);
		EntityComponentSystem::update_components(entity, msg.components);
	}

	void operator()(MsgRemoveComponents const &msg) {

		entity_t entity = controller.getEntity(msg.entity_id);
		for (uint32_t idx : msg.components) {
			entity.sync();
			EntityComponentSystem::removeComponent(entity, idx);
		}
	}

	void operator()(MsgDeleteEntity const &msg) {

		entity_t entity = controller.getEntity(msg.entity_id);
		controller.mEntites.erase(msg.entity_id);
		entity.destroy();
	}

	void operator()(MsgGameInfo const &msg) {

		controller.mGameInfo = msg.gameInfo;
		controller.mClientGui.setTeamInfo(msg.gameInfo.teams);
		controller.mClientGui.setMapSize(msg.gameInfo.width(),
		                                 msg.gameInfo.height());
	}

	void operator()(MsgPlayerAssignedEntityId const &msg) {

		controller.mPlayerId = msg.entityId;
		controller.mPlayerEntity = controller.getEntity(controller.mPlayerId);
	}

private:
	ClientController &controller;
};


ClientController::ClientController(PlayerInfo &&player, ClientGui &clientGui,
                                   ConnectionToServer &server)
    : mClientGui(clientGui), mRemoteServer{server}, mPlayerInfo(player) {}

void ClientController::start() {

  const auto constexpr dt = 16ms;
  const auto clientStartPoint = chrono::steady_clock::now();
  auto clientGameTime = clientStartPoint;
  bool const limit_speed = true;

	ClientMessage msg{MsgIntroduceMyPlayer{mPlayerInfo}};
	mRemoteServer.send(msg.to_message());

  while (!mEndLoop) {

	  mRemoteServer.poll();
    loopWork();

    clientGameTime += dt;
    if (limit_speed) {
	    this_thread::sleep_until(clientGameTime);
    }
  }

  cout << "Quitting.\n";
}

void ClientController::stop() { mEndLoop = true; }

void ClientController::loopWork() {

  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {

    if (e.type == SDL_QUIT) {

      mEndLoop = true;
      break;
    }
  }

	sendUserInteractionToServer();
	mClientGui.renderGui(mEcs);
}

void ClientController::received(Message msg) {

	if (msg.tag == Tag::UniversalServerMessage) {
		return received(ServerMessage::from(msg));
	}
}

void ClientController::received(ServerMessage msg) {

  Receiver receiver{*this};
  boost::apply_visitor(receiver, msg.data);
}

void ClientController::send(ClientMessage const &msg) {

  mRemoteServer.send(msg.to_message());
}

void ClientController::sendUserInteractionToServer() {

	// Keyboard
	const Uint8 *keyboard = SDL_GetKeyboardState(NULL);

	for (auto &keyMapping : keyMap) {

		bool prevState = keyMapping.second.second;
		bool curState = keyboard[keyMapping.first];

		if (prevState != curState) {

			keyMapping.second.second = curState;
			send({MsgPlayerActionChange{keyMapping.second.first, curState}});
		}
	}

	// Mouse

	int mouseX;
	int mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	// TODO this might be very slow, maybe I should cach a reference

	auto entityIt = mEntites.find(mPlayerId);
	if (entityIt != mEntites.end()) {

		auto entity = entityIt->second;
		if (!entity.has_component<Position>()) {
			return;
		}
		geometry::Point point = entity.get_component<Position>().center;

		point = mClientGui.getScreenCoords(point);

		int x1 = 0;
		int y1 = -1;
		int x2 = mouseX - point.x;
		int y2 = mouseY - point.y;

		double dot = x1 * x2 + y1 * y2;
		double det = x1 * y2 - y1 * x2;
		geometry::Angle angle = atan2(det, dot) * 180 / M_PI;

		if (angle < 0) {
			angle += 360;
		}

		ClientMessage mouseMsg = {MsgPlayerRotation{angle}};
		send(mouseMsg);
	}
}

entity_t ClientController::getMyPlayer() {

  if (mPlayerEntity.get_status() != entityplus::entity_status::UNINITIALIZED) {
	  mPlayerEntity.sync();
  }

  return mPlayerEntity;
}

entity_t ClientController::getEntity(const EntityID &id) {

  entity_t &entity = mEntites.at(id);
  entity.sync();
  return entity;
}

bool ClientController::isMyPlayer(const EntityID &id) const { return id == mPlayerId; }