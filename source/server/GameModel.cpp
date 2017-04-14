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

GameModel::GameModel(GameObjectManager &gameObjects, IBroadcaster & broadcaster) :
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
	cout << "Game model" << endl;

	auto const dt = 16ms;

	while(!stop) {
		realTime = chrono::steady_clock::now();
		//cout << "Now: " <<std::chrono::duration_cast<std::chrono::microseconds>(realTime - startPoint).count() << endl;

		do_physics(dt);

		gameTime = gameTime + dt;
		this_thread::sleep_until(gameTime);
	}
}

// TODO we should measure the diffeence between client's and server's time.

void GameModel::do_physics(std::chrono::milliseconds dt) {
	for (GameObject & gameObject : gameObjects) {
		if (gameObject.speed != Vector{0, 0}) {
			gameObject.center += gameObject.speed * Scalar(dt.count() / 1000.0);
			broadcaster.notify(gameObject);
		}
	}
}
