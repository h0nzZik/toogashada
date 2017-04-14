#include <chrono>
#include <iostream>
#include <ctime>

#include <server/GameModel.h>

using namespace std;
using namespace std::chrono_literals;

GameModel::GameModel() :
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

	auto clock_step = 16ms;

	while(!stop) {
		realTime = chrono::steady_clock::now();
		//cout << "Now: " <<std::chrono::duration_cast<std::chrono::microseconds>(realTime - startPoint).count() << endl;

		// TODO do the physics

		gameTime = gameTime + clock_step;
		this_thread::sleep_until(gameTime);
	}
}
