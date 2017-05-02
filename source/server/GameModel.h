#pragma once

#include <memory> // std::unique_ptr

struct EntityComponentSystem;
class IBroadcaster;
struct SEntity;

class GameModel {
public:
	GameModel(EntityComponentSystem & ecs, IBroadcaster & broadcaster);
	~GameModel();

	EntityComponentSystem & ecs;
	SEntity newPlayer();
	void newBullet();
	void removeEntity(SEntity const &entity);
	const geometry::RectangularArea &getMapSize();
private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

