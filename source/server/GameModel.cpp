// Standard
#include <chrono>
#include <iostream>
#include <ctime>

// Server
#include "IBroadcaster.h"

// Self
#include "GameModel.h"

using namespace std;
using namespace std::chrono_literals;

GameModel::GameModel(GameObjectManager &gameObjects, EntityComponentSystem & ecs, IBroadcaster & broadcaster) :
	ecs{ecs},
	gameObjects{gameObjects},
	broadcaster{broadcaster},
	runner{&GameModel::main, this}
{

}

GameModel::~GameModel() {
	stop = true;
	runner.join();
}

void GameModel::main() {
	stop = false;
	startPoint = realTime = gameTime = chrono::steady_clock::now();

	auto const dt = 16ms;

	while(!stop) {
		realTime = chrono::steady_clock::now();

		do_physics(dt);

		gameTime = gameTime + dt;
		this_thread::sleep_until(gameTime);
	}
}

// TODO we should measure the diffeence between client's and server's time.

void GameModel::do_physics(std::chrono::milliseconds dt) {
	ecs.entityManager.for_each<Position>([&](entity_t entity, Position & pos){
		if (pos.speed != Vector{0, 0}) {
			pos.center += pos.speed * Scalar(dt.count() / 1000.0);
			// TODO some notification
			// Muzeme mit zpravu o updatu libovolne komponenty...
			// Potrebuji poslat tuhle komponentu - pozici.
			//
		}
	});

	for (GameObject & gameObject : gameObjects) {
		if (gameObject.speed != Vector{0, 0}) {
			gameObject.center += gameObject.speed * Scalar(dt.count() / 1000.0);
			broadcaster.notify(gameObject);
		}
	}
}


