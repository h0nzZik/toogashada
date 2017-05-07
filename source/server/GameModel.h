#pragma once

// standard
#include <memory> // std::unique_ptr

// common
#include <common/geometry/Angle.h>

struct EntityComponentSystem;
class IBroadcaster;
struct SEntity;
enum class PlayerAction;

class GameModel {
public:
  GameModel(EntityComponentSystem &ecs, IBroadcaster &broadcaster);
  ~GameModel();

  SEntity newPlayer(PlayerInfo playerInfo);
  void removeEntity(SEntity const &entity);
  void playerKeyPress(SEntity const &entity, PlayerAction key, bool press);
  void playerRotatesTo(SEntity const &entity, geometry::Angle angle);
  const GameInfo &getGameInfo();
  void stop();
  void main();

private:
  class Impl;
  std::unique_ptr<Impl> pImpl;
};
