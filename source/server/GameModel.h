#pragma once

#include <thread>
#include <atomic>
#include <chrono>

#include <common/GameObjectManager.h>

class IBroadcaster;

class GameModel {
public:
	GameModel(GameObjectManager &gameObjects, IBroadcaster & broadcaster);
	~GameModel();

private:
	void main();
	void do_physics(std::chrono::milliseconds dt);

	GameObjectManager &gameObjects;
	IBroadcaster & broadcaster;

	std::thread runner;
	std::atomic<bool> stop;
	std::chrono::steady_clock::time_point startPoint;
	std::chrono::steady_clock::time_point realTime;
	std::chrono::steady_clock::time_point gameTime;
};

