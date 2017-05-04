#pragma once

#include <memory> // std::unique_ptr

struct EntityComponentSystem;
class IBroadcaster;
struct SEntity;
enum class PlayerAction;

class GameModel {
public:
	GameModel(EntityComponentSystem & ecs, IBroadcaster & broadcaster);
	~GameModel();

	EntityComponentSystem & ecs;
	SEntity newPlayer();
	void newBullet();
	void removeEntity(SEntity const &entity);
	void playerKeyPress(SEntity const &entity, PlayerAction key, bool press);
	const geometry::RectangularArea &getMapSize();
private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

