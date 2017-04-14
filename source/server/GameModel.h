#pragma once

#include <thread>
#include <atomic>
#include <chrono>

class GameModel {
public:
	GameModel();
	~GameModel();

private:
	void main();
	std::thread runner;
	std::atomic<bool> stop;
	std::chrono::steady_clock::time_point startPoint;
	std::chrono::steady_clock::time_point realTime;
	std::chrono::steady_clock::time_point gameTime;
};

