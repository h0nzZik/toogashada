#pragma once

#include <memory> // std::unique_ptr

struct EntityComponentSystem;
class IBroadcaster;

class GameModel {
public:
	GameModel(EntityComponentSystem & ecs, IBroadcaster & broadcaster);
	~GameModel();

	EntityComponentSystem & ecs;
private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

