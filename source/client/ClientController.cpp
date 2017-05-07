// standard
#include <iostream>
#include <map>
#include <math.h>
#include <memory> // make_unique
#include <mutex>
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

#include "ClientGui.h"
#include "common/components/PlayerInfo.h"
#include "RemoteServerWrapper.h"

// self
#include "ClientController.h"

using namespace std;

ClientController::ClientController(PlayerInfo &player, ClientGui &clientGui,
                                   RemoteServerWrapper &server)
        : clientGui(clientGui), remoteServer{server}, player(player),
          continueLoop{true} {}

void ClientController::main_loop() {
    const auto constexpr dt = 16ms;
    const auto clientStartPoint = chrono::steady_clock::now();
    auto clientGameTime = clientStartPoint;
    bool const limit_speed = true;
    while (continueLoop) {

        loopWork();

        clientGameTime += dt;
        if (limit_speed)
            this_thread::sleep_until(clientGameTime);
    }
    cout << "Quitting gui\n";
}

void ClientController::stop() { continueLoop = true; }

void ClientController::loopWork() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            continueLoop = false;
            break;
        }
    }

    dispatchKeyAndMouseStates();

    redraw();
}

class ClientController::Receiver : public boost::static_visitor<void> {
public:
    Receiver(ClientController &controller) : controller(controller) {}

    void operator()(MsgNewPlayer const /*&msg*/) { ; }

    void operator()(MsgNewEntity const &msg) {
        std::lock_guard<std::mutex> guard{controller.mutexGameObjects};
        controller.entites[msg.entity_id] =
                controller.ecs.entityManager.create_entity(msg.entity_id);
        entity_t entity = controller.getEntity(msg.entity_id);
        EntityComponentSystem::add_components(entity, msg.components);
    }

    void operator()(MsgUpdateComponents const &msg) {
        std::lock_guard<std::mutex> guard{controller.mutexGameObjects};
        entity_t entity = controller.getEntity(msg.entity_id);
        EntityComponentSystem::update_components(entity,
                                                 msg.components);
    }

    void operator()(MsgRemoveComponents const &msg) {
        std::lock_guard<std::mutex> guard{controller.mutexGameObjects};
        entity_t entity = controller.getEntity(msg.entity_id);
        for(uint32_t idx : msg.components) {
        	cout << "Going to remove component" << endl;
        	entity.sync();
        	EntityComponentSystem::removeComponent(entity, idx);
        }
    }

    void operator()(MsgDeleteEntity const &msg) {
        std::lock_guard<std::mutex> guard{controller.mutexGameObjects};
        // TODO separate class for managing the map
        entity_t entity = controller.getEntity(msg.entity_id);
        controller.entites.erase(msg.entity_id);
        entity.destroy();
    }

    void operator()(MsgGameInfo const &msg) {

        controller.gameInfo = msg.gameInfo;
        controller.clientGui.setTeamInfo(msg.gameInfo.teams);
        controller.clientGui.setMapSize(msg.gameInfo.width(), msg.gameInfo.height());
        std::cout << "map size set to: " << msg.gameInfo.width() << "x"
                  << msg.gameInfo.height() << endl;
    }

    void operator()(MsgPlayerAssignedEntityId const &msg) {
        std::lock_guard<std::mutex> guard{controller.mutexGameObjects};
        controller.playerId = msg.entityId;
        controller.playerEntity = controller.getEntity(controller.playerId);
    }

private:
    ClientController &controller;
};

void ClientController::received(Message msg) {

    switch (msg.tag) {
        case Tag::Hello:
            cout << "Server says hello." << endl;
            break;

        case Tag::UniversalServerMessage:
            return received(ServerMessage::from(msg));

        default:
            break;
    }
}

void ClientController::received(ServerMessage msg) {
    Receiver receiver{*this};
    boost::apply_visitor(receiver, msg.data);
}

void ClientController::redraw() {
    std::lock_guard<std::mutex> guard{mutexGameObjects};
    // TODO(h0nzZik): I think it is not needed to lock everything.
    // We should refine it someday in future.

    clientGui.renderGui(ecs);
}

/**
 * A note about how we compute object positions.
 * Server periodically sends the authoritative information about object
 * positions.
 * However
 */

void ClientController::send(ClientMessage const &msg) {
    remoteServer.conn().send(msg.to_message());
}

void ClientController::dispatchKeyStates() {
    ClientMessage msg;
    const Uint8 *keyboard = SDL_GetKeyboardState(NULL);

    for (auto &keyMapping : keyMap) {

        bool prevState = keyMapping.second.second;
        bool curState = keyboard[keyMapping.first];

        if (prevState != curState) {
            msg = {MsgPlayerActionChange{keyMapping.second.first, curState}};
            keyMapping.second.second = curState;
            send(msg);
            // std::cout <<
            // static_cast<std::underlying_type<Movement>::type>(keyMapping.second.first)
            // << " " << prevState << " -> " << curState << std::endl;
        }
    }
}

void ClientController::dispatchKeyAndMouseStates() {
    dispatchKeyStates();
    ClientMessage msg;

    int mouseX;
    int mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // TODO this might be very slow, maybe I should cach a reference
    // TODO improve semantic

    {
        std::unique_lock<std::mutex> guard{mutexGameObjects};

        if (entites.find(playerId) != entites.end()) {
        	entity_t playerEntity = getEntity(playerId);
            geometry::Point point =
            		playerEntity.get_component<Position>().center;
            guard.unlock();

            point = clientGui.getEntityMapRefPoint(point);

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
}

entity_t ClientController::getMyPlayer() {
	if (playerEntity.get_status() != entityplus::entity_status::UNINITIALIZED)
		playerEntity.sync();
	return playerEntity;
}

entity_t ClientController::getEntity(const EntityID &id) {
	entity_t & entity = entites.at(id);
	entity.sync();
	return entity;
}

bool ClientController::isMyPlayer(const EntityID &id) { return id == playerId; }
