// Standard
#include <chrono>
#include <iostream>
#include <ctime>
#include <thread>
#include <atomic>

#include <common/GameObjectManager.h>
#include <common/EntityComponentSystem.h>

// Server
#include "IBroadcaster.h"

// Self
#include "GameModel.h"

using namespace std;
using namespace std::chrono_literals;

class GameModel::Impl {
public:
	Impl(GameObjectManager &gameObjects, EntityComponentSystem & ecs, IBroadcaster & broadcaster) :
		ecs{ecs},
		gameObjects{gameObjects},
		broadcaster{broadcaster},
		runner{&Impl::main, this}
	{ }

	~Impl() {
		stop = true;
		runner.join();
	}

	EntityComponentSystem & ecs;
private:

	// TODO we should measure the diffeence between client's and server's time.

	void main() {
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

	void do_physics(std::chrono::milliseconds dt) {
		ecs.entityManager.for_each<Position>([&](entity_t const & entity, Position & pos){
			if (pos.speed != Vector{0, 0}) {
				pos.center += pos.speed * Scalar(dt.count() / 1000.0);
				broadcaster.notify(entity, {pos});
				// TODO some notification
				// Muzeme mit zpravu o updatu libovolne komponenty...
				// Potrebuji poslat tuhle komponentu - pozici.
				//
			}
		});

#if 0
		for (GameObject & gameObject : gameObjects) {
			if (gameObject.speed != Vector{0, 0}) {
				gameObject.center += gameObject.speed * Scalar(dt.count() / 1000.0);
				broadcaster.notify(gameObject);
			}
		}
#endif
	}

	GameObjectManager &gameObjects;
	IBroadcaster & broadcaster;

	std::thread runner;
	std::atomic<bool> stop;
	std::chrono::steady_clock::time_point startPoint;
	std::chrono::steady_clock::time_point realTime;
	std::chrono::steady_clock::time_point gameTime;
};

GameModel::GameModel(GameObjectManager &gameObjects, EntityComponentSystem & ecs, IBroadcaster & broadcaster) :
		ecs(ecs)
{
	pImpl = make_unique<Impl>(gameObjects, ecs, broadcaster);
}

GameModel::~GameModel() = default;


