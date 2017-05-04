// Standard
#include <atomic>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <random>
#include <thread>

#include <common/EntityComponentSystem.h>
#include <common/Geometry.h>
#include <common/PlayerAction.h>
#include <common/ServerMessage.h>
#include <common/geometry/Object2D.h>
#include <common/geometry/collision.h>

// Server
#include "IBroadcaster.h"
#include "SEntity.h"

// Self
#include "GameModel.h"

using namespace std;
using namespace std::chrono_literals;
using namespace geometry;

class GameModel::Impl {
public:
  Impl(EntityComponentSystem &ecs, IBroadcaster &broadcaster)
      : ecs{ecs}, broadcaster{broadcaster}, runner{&Impl::main, this} {}

  ~Impl() {
    stop = true;
    runner.join();
  }

  geometry::CircleShape const playerShape = {10};

  struct CollisionInfo {
    bool happened = false;
    entity_t with;

    operator bool() const { return happened; }
  };

  // TODO rather return a vector of colliding entites
  CollisionInfo collidesWithSomething(geometry::Object2D const &object,
                                      entity_t const *entity = nullptr) {
    ;
    CollisionInfo info;
    info.happened = false;
    ecs.entityManager.for_each<geometry::Object2D>(
        [&](auto oldEntity, geometry::Object2D const &oldObject) {
          if (info.happened)
            return;

          if (entity && (oldEntity == *entity))
            return;

          if (geometry::collision(object, oldObject)) {
            info.happened = true;
            info.with = oldEntity;
          }
        });

    return info;
  }

  void removeEntity(entity_t entity) {
    ServerMessage msg{MsgDeleteEntity{entity.get_component<EntityID>()}};
    broadcaster.broadcast(msg.to_message());
    entity.destroy();
  }

  std::random_device d{};
  std::default_random_engine randomEngine{d()};

  geometry::Point randomPoint() {
    return {Scalar(randomEngine() % int(game_area.bottomRight().x)),
            Scalar(randomEngine() % int(game_area.bottomRight().y))};
  }

  const geometry::RectangularArea &getMapSize() { return game_area; }

  entity_t newEntity() {
    return ecs.entityManager.create_entity(EntityID::newID());
  }

  void playerShoots(Position const &playerPosition) {
    entity_t entity = newEntity();
    Position pos;
    log() << "Shooting at angle " << playerPosition.rotation;
    pos.center = playerPosition.center +
                 (1 + bullet_r + playerShape.radius) *
                     geometry::rotate({0, -1}, playerPosition.rotation);
    auto speedVector = geometry::rotate(pos.center - playerPosition.center,
                                        playerPosition.rotation);
    pos.speed = 20 * unit(pos.center - playerPosition.center);
    cout << "Speed: " << pos.speed;
    entity.add_component<Position>(pos);
    entity.add_component<Shape>(geometry::CircleShape{bullet_r});
    entity.add_component<Explosive>();
    computeObjectIfNeeded(entity);
    entity.sync();

    MsgNewEntity msg;
    msg.components = EntityComponentSystem::all_components(entity);
    msg.entity_id = entity.get_component<EntityID>();
    ServerMessage sm{msg};
    broadcaster.broadcast(sm.to_message());
  }

  void computeObjectIfNeeded(entity_t entity) {
    if (!entity.has_component<Position>())
      return;
    if (!entity.has_component<Shape>())
      return;
    if (entity.has_component<geometry::Object2D>())
      return;

    Shape const &shape = entity.get_component<Shape>();
    Position const &position = entity.get_component<Position>();
    entity.add_component<geometry::Object2D>(
        geometry::createObject2D(position.center, position.rotation, shape));
  }

  SEntity newPlayer() {
    entity_t entity = newEntity();
    Position pos;

    // Hope it will eventually find a good place.
    // TODO collision with other players
    // TODO counter. We might reject the player.
    int counter = 0;
    geometry::Object2D object2d;

    // TODO refacto this to separate function
    CollisionInfo colInfo;
    do {
      pos.center = randomPoint();
      object2d = createObject2D(pos.center, 0, playerShape);
      colInfo = collidesWithSomething(object2d);

    } while (++counter < 50 &&
             (colInfo || !geometry::in(pos.center, playerShape, game_area)));

    if (counter >= 50)
      throw std::runtime_error("Cannot fit player to area");

    log() << "Found place for player on " << counter << "th try";

    entity.add_component<Position>(pos);
    // entity.add_component<Shape>(CircleShape{5.1});
    entity.add_component<Shape>(playerShape);
    entity.add_component<geometry::Object2D>(std::move(object2d));
    entity.add_component<PlayerInfo>();

    return SEntity{entity};
  }

  EntityComponentSystem &ecs;

  void playerRotatesTo(entity_t entity, geometry::Angle angle) {
    entity.sync();
    if (!entity.has_component<Position>()) {
      throw std::logic_error("Player is missing some components");
    }

    Position &pos = entity.get_component<Position>();
    pos.rotation = angle;
  }

  void playerKeyPress(entity_t entity, PlayerAction key, bool press) {
    entity.sync();
    if (!entity.has_component<Position>() ||
        !entity.has_component<PlayerInfo>()) {
      throw std::logic_error("Player is missing some components");
    }

    Position &position = entity.get_component<Position>();
    PlayerInfo &playerInfo = entity.get_component<PlayerInfo>();
    switch (key) {
    case PlayerAction::Down:
      playerInfo.down = press;
      break;

    case PlayerAction::Up:
      playerInfo.up = press;
      break;

    case PlayerAction::Left:
      playerInfo.left = press;
      break;

    case PlayerAction::Right:
      playerInfo.right = press;
      break;

    case PlayerAction::Fire:
      if (press)
        playerShoots(position);
      break;

    default:
      throw std::runtime_error("Unknown player action");
    }
    position.speed = toSpeedVector(playerInfo);
  }

private:
  static constexpr Scalar bullet_r = 1.5;
  static constexpr Scalar player_r = 10;
  static geometry::Vector toSpeedVector(PlayerInfo const &info) {
    Vector v{0, 0};
    if (info.down)
      v += Vector{0, +1};
    if (info.left)
      v += Vector{-1, 0};
    if (info.right)
      v += Vector{+1, 0};
    if (info.up)
      v += Vector{0, -1};
    return v * Scalar(15);
  }

  geometry::RectangularArea const game_area{{0, 0}, {100, 100}};
  // TODO we should measure the diffeence between client's and server's time.

  static auto constexpr dt = 16ms;
  void main() {
    stop = false;
    startPoint = realTime = gameTime = chrono::steady_clock::now();

    while (!stop) {
      realTime = chrono::steady_clock::now();

      do_physics();

      gameTime = gameTime + dt;
      this_thread::sleep_until(gameTime);
    }
  }

  void do_physics() {
    using namespace std::placeholders;
    ecs.entityManager.for_each<Position, Shape, geometry::Object2D>(
        std::bind(&Impl::update_position, this, _1, _2, _3, _4));
  }

  void explosiveCollision(entity_t explosive_entity) {
    log() << "Explosive collision";
    removeEntity(explosive_entity);
  }

  void collisionHappened(entity_t a, Position &posa, entity_t b) {
    posa.speed = geometry::Vector{0, 0};
    if (a.has_component<Explosive>())
      explosiveCollision(a);
    // if (b.has_component<Explosive>())
    //	explosiveCollision(b);
  }

  void collisionHappenedWithArea(entity_t entity, Position &pos) {
    pos.speed = geometry::Vector{0, 0};

    if (entity.has_component<Explosive>())
      explosiveCollision(entity);
  }

  void update_position(entity_t const &entity, Position &pos,
                       Shape const &shape, geometry::Object2D &oldObject) {
    if (std::fabs(pos.speed.x) < 0.01 && std::fabs(pos.speed.y) < 0.01)
      return;

    geometry::Point const new_center =
        pos.center + pos.speed * Scalar(dt.count() / 1000.0);
    auto currentObject2d = createObject2D(new_center, pos.rotation, shape);

    CollisionInfo colInfoNow = collidesWithSomething(currentObject2d, &entity);

    if (colInfoNow) {
      log() << "Entity " << entity.get_component<EntityID>().id()
            << " have collision now";
      log() << "  with: " << colInfoNow.with.get_component<EntityID>().id();
    }

    if (!in(new_center, shape, game_area))
      return collisionHappenedWithArea(entity, pos);

    if (colInfoNow)
      return collisionHappened(entity, pos, colInfoNow.with);

    pos.center = new_center;
    oldObject = currentObject2d;
    broadcaster.updateEntity(entity, {pos});
  }

  class Log {
    ostream &o;

  public:
    explicit Log(ostream &o) : o(o) {}
    ~Log() { o << endl; }

    operator ostream &() { return o; }

    template <typename T> ostream &operator<<(T const &x) {
      o << x;
      return o;
    }
  };
  Log log() {
    cout << "[GameModel, " << (gameTime - startPoint).count() << "]";
    return Log(cout);
  }

  IBroadcaster &broadcaster;

  std::thread runner;
  std::atomic<bool> stop;
  std::chrono::steady_clock::time_point startPoint;
  std::chrono::steady_clock::time_point realTime;
  std::chrono::steady_clock::time_point gameTime;
};

const std::chrono::milliseconds GameModel::Impl::dt;

GameModel::GameModel(EntityComponentSystem &ecs, IBroadcaster &broadcaster)
    : ecs(ecs) {
  pImpl = make_unique<Impl>(ecs, broadcaster);
}

GameModel::~GameModel() = default;

SEntity GameModel::newPlayer() { return pImpl->newPlayer(); }

void GameModel::removeEntity(SEntity const &entity) {
  return pImpl->removeEntity(entity.entity);
}

const geometry::RectangularArea &GameModel::getMapSize() {
  return pImpl->getMapSize();
}

void GameModel::playerKeyPress(SEntity const &entity, PlayerAction key,
                               bool press) {
  return pImpl->playerKeyPress(entity.entity, key, press);
}

void GameModel::playerRotatesTo(SEntity const &entity, geometry::Angle angle) {
  pImpl->playerRotatesTo(entity.entity, angle);
}
