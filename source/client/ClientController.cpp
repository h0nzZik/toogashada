//standard
#include <iostream>
#include <memory> // make_unique
#include <mutex>
#include <map>
#include <thread>
#include <math.h>

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

#include "ClientPlayer.h"
#include "ClientGui.h"
#include "RemoteServerWrapper.h"


// self
#include "ClientController.h"

using namespace std;

ClientController::ClientController(ClientPlayer &player, ClientGui &clientGui, RemoteServerWrapper &server) :
        clientGui(clientGui), remoteServer{server},
        player(player), continueLoop{true} {}



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

void ClientController::stop() {
    continueLoop = true;
}

void ClientController::loopWork() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT) {
            continueLoop = false;
            break;
        }
    }

    dispatchKeyStates();

    redraw();
}


class ClientController::    Receiver : public boost::static_visitor<void> {
public:
    Receiver(ClientController &controller) : controller(controller) {}

    void operator()(MsgNewPlayer const &msg) {
        ;
    }

    void operator()(MsgNewEntity const &msg) {
        std::lock_guard<std::mutex> guard{controller.mutexGameObjects};
        controller.entites[msg.entity_id] = controller.ecs.entityManager.create_entity(msg.entity_id);
        auto &entity = controller.entites[msg.entity_id];
        EntityComponentSystem::add_components(entity, msg.components);
    }

    void operator()(MsgUpdateEntity const &msg) {
        std::lock_guard<std::mutex> guard{controller.mutexGameObjects};
        EntityComponentSystem::update_components(controller.entites[msg.entity_id], msg.components);
    }

    void operator()(MsgDeleteEntity const &msg) {
        std::lock_guard<std::mutex> guard{controller.mutexGameObjects};
        controller.entites[msg.entity_id].destroy();
    }

    void operator()(MsgGameInfo const &msg) {

        controller.clientGui.setMapSize(msg.area_size_x, msg.area_size_y);
        std::cout << "map size set to: " << msg.area_size_x << "x" << msg.area_size_y << endl;
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
 * Server periodically sends the authoritative information about object positions.
 * However
 */


void ClientController::send(ClientMessage const &msg) {
    remoteServer.conn().send(msg.to_message());
}

void ClientController::dispatchKeyStates() {


    ClientMessage msg;

    const Uint8* keyboard = SDL_GetKeyboardState(NULL);


    for (auto &keyMapping : keyMap) {

        bool prevState = keyMapping.second.second;
        bool curState = keyboard[keyMapping.first];

        if (prevState != curState) {
            msg = {MsgPlayerActionChange{keyMapping.second.first, curState}};
            keyMapping.second.second = curState;
            send(msg);
            //std::cout << static_cast<std::underlying_type<Movement>::type>(keyMapping.second.first) << " " << prevState << " -> " << curState << std::endl;
        }
    }

    int mouseX;
    int mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    //std::cout << mouseX << ", " << mouseY << std::endl;

    int posX = 500,
        posY = 500;

    int x1 = 0;
    int y1 = -1;

    int x2 = mouseX - posX;
    int y2 = mouseY - posY;

    double dot = x1*x2 + y1*y2;     
    double det = x1*y2 - y1*x2;
    double angle = atan2(det, dot) * 180 / M_PI;

    if (angle < 0.0) {
        angle += 360.0;
    }

    ClientMessage mouseMsg = {MsgPlayerRotation{angle}};
    send(mouseMsg);
}

