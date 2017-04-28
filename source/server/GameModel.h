#pragma once

#include <memory> // std::unique_ptr

class GameObject;
template < typename T > class GenericManager;
using GameObjectManager = GenericManager<GameObject>;

struct EntityComponentSystem;
class IBroadcaster;

class GameModel {
public:
	GameModel(GameObjectManager &gameObjects, EntityComponentSystem & ecs, IBroadcaster & broadcaster);
	~GameModel();

	EntityComponentSystem & ecs;
private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

