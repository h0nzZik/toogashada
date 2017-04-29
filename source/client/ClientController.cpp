//standard
#include <iostream>
#include <memory> // make_unique
#include <mutex>
#include <map>
#include <thread>

// SDL
#include <SDL.h>

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


class ClientController::Impl final {
public:
	Impl(ClientPlayer &player, ClientGui & clientGui, RemoteServerWrapper & server) :
		clientGui(clientGui),remoteServer{server},
		player(player),	quit{false}
	{

    }
	~Impl() = default;

	void received(Message msg);
	void main_loop() {
		const auto constexpr dt = 16ms;
		const auto clientStartPoint = chrono::steady_clock::now();
		auto clientGameTime = clientStartPoint;
		bool const limit_speed = true;
		while( !quit )
		{
			iter();
			clientGameTime += dt;
			if (limit_speed)
				this_thread::sleep_until(clientGameTime);
		}
		cout << "Quitting gui\n";
	}

	void stop() {
		quit = true;
	}

private:
	void iter() {
		SDL_Event e;
		while( SDL_PollEvent( &e ) != 0 )
			handle_event(e);

		redraw();
	}

	class Receiver;

	void redraw();
	void received(ServerMessage msg);
	void handle_event(SDL_Event const & e);
	void handleKeyPress(SDL_Scancode code);
	void handleKeyRelease(SDL_Scancode code);
	void send(ClientMessage const & msg);

	ClientGui & clientGui;
	RemoteServerWrapper & remoteServer;
	ClientPlayer &player;

	bool quit;

	//GameObjectManager gameObjects;
	std::mutex mutexGameObjects;

	struct PressedKeys {
		bool key_up = false;
		bool key_down = false;
		bool key_left = false;
		bool key_right = false;
	};

	PressedKeys pressedKeys;

	Config config;
	EntityComponentSystem ecs;
	std::map<EntityID, entity_t> entites;
};


class ClientController::Impl::Receiver : public boost::static_visitor<void> {
	public:
		Receiver(ClientController::Impl & controller) : controller(controller) {}

		void operator()(MsgNewPlayer const & msg) {
			;
		}

		void operator()(MsgNewEntity const & msg) {
			std::lock_guard<std::mutex> guard{controller.mutexGameObjects};
			//cout << "Received new entity " << msg.entity_id.id() << endl;
			controller.entites[msg.entity_id] = controller.ecs.entityManager.create_entity(msg.entity_id);
			auto & entity = controller.entites[msg.entity_id];
			EntityComponentSystem::add_components(entity, msg.components);
		}

		void operator()(MsgUpdateEntity const & msg) {
			//cout << "Received update of entity " << msg.entity_id.id() << endl;
			EntityComponentSystem::update_components(controller.entites[msg.entity_id], msg.components);
		}

	private:
		ClientController::Impl & controller;
};

void ClientController::Impl::received(Message msg) {

	switch(msg.tag) {
		case Tag::Hello:
			cout << "Server says hello." << endl;
			break;

		case Tag::UniversalServerMessage:
			return received(ServerMessage::from(msg));

		default:
			break;
	}
}

void ClientController::Impl::received(ServerMessage msg) {
	Receiver receiver{*this};
	boost::apply_visitor(receiver, msg.data);
}

void ClientController::Impl::redraw() {
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

void ClientController::Impl::handle_event(SDL_Event const & e) {
	switch(e.type) {
		case SDL_QUIT:
			quit = true;
			break;

		case SDL_WINDOWEVENT:
			//cout << "Window event" << endl;
			break;

		case SDL_MOUSEMOTION:
			// cout << "Mouse at [" << e.motion.x << "," << e.motion.y << "]" << endl;
			break;

		case SDL_KEYDOWN:
			if (e.key.repeat == 0)
				handleKeyPress(e.key.keysym.scancode);
			break;

		case SDL_KEYUP:
			handleKeyRelease(e.key.keysym.scancode);
			break;

		default:
			break;
	}
}

void ClientController::Impl::send(ClientMessage const & msg) {
	remoteServer.conn().send(msg.to_message());
}

void ClientController::Impl::handleKeyPress(SDL_Scancode code) {
	ClientMessage msg;

	if (code == config.key_down)
	{
		msg = {MsgPlayerMovesBackward{}};
		pressedKeys.key_down = true;
	}
	else if (code == config.key_left)
	{
		msg = {MsgPlayerMovesLeft{}};
		pressedKeys.key_left = true;
	}
	else if (code == config.key_right)
	{
		msg = {MsgPlayerMovesRight{}};
		pressedKeys.key_right = true;
	}
	else if (code == config.key_up)
	{
		msg = {MsgPlayerMovesForward{}};
		pressedKeys.key_up = true;
	}
	else
		return;

	send(msg);
}

void ClientController::Impl::handleKeyRelease(SDL_Scancode code) {
	if (code == config.key_down)
	{
		pressedKeys.key_down = false;
	}
	else if (code == config.key_left)
	{
		pressedKeys.key_left = false;
	}
	else if (code == config.key_right)
	{
		pressedKeys.key_right = false;
	}
	else if (code == config.key_up)
	{
		pressedKeys.key_up = false;
	}
	else
		return;

	if (!pressedKeys.key_down && !pressedKeys.key_up &&
			!pressedKeys.key_left && !pressedKeys.key_right) {
		ClientMessage m{MsgPlayerStops{}};
		remoteServer.conn().send(m.to_message());
	}
}

ClientController::ClientController(
		ClientPlayer &player,
		ClientGui & clientGui,
		RemoteServerWrapper & server) {
	impl = make_unique<Impl>(player, clientGui, server);
}

ClientController::~ClientController() = default;


void ClientController::received(Message msg) {
	impl->received(std::move(msg));
}

void ClientController::main_loop() {
	impl->main_loop();
}

void ClientController::stop() {
	impl->stop();
}

ClientController::Config::Config()
{
	key_up = SDL_SCANCODE_W;
	key_down = SDL_SCANCODE_S;
	key_left = SDL_SCANCODE_A;
	key_right = SDL_SCANCODE_D;
}


