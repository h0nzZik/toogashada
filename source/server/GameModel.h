#pragma once

#include <common/geometry/Angle.h>
#include <memory> // std::unique_ptr

struct EntityComponentSystem;
class IBroadcaster;
struct SEntity;
enum class PlayerAction;

class GameModel {
public:
  GameModel(EntityComponentSystem &ecs, IBroadcaster &broadcaster);
  ~GameModel();

  EntityComponentSystem &ecs;
  SEntity newPlayer();
  void removeEntity(SEntity const &entity);
  void playerKeyPress(SEntity const &entity, PlayerAction key, bool press);
  void playerRotatesTo(SEntity const &entity, geometry::Angle angle);
  const geometry::RectangularArea &getMapSize();
  void stop();
  void main();

private:
  class Impl;
  std::unique_ptr<Impl> pImpl;
};
