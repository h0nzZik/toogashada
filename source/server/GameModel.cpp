// Standard
#include <atomic>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <random>
#include <thread>

// Common
#include <common/EntityComponentSystem.h>
#include <common/GameInfo.h>
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
      : ecs{ecs}, broadcaster{broadcaster} {
    generateMap();
	auto seed = d();
	cout << "GameModel seed: " << seed << endl;
	// TODO it should be possible to specify the seed on command line.
	randomEngine = std::default_random_engine(seed);
  }

  ~Impl() { m_stop = true; }

  vector<entity_t>
  collidesWithSomething(geometry::Object2D const &object,
                        entity_t const *ignore_this = nullptr) {
    vector<entity_t> colliding;
    ecs.entityManager.for_each<geometry::Object2D>(
        [&](entity_t oldEntity, geometry::Object2D const &oldObject) {
          if (ignore_this && (oldEntity == *ignore_this))
            return;

          if (geometry::collision(object, oldObject))
            colliding.push_back(oldEntity);
          else if (in(object, oldObject))
        	  colliding.push_back(oldEntity);
        });

    return colliding;
  }

  void removePlayer(const PlayerInfo &info) { gameInfo.removePlayer(info); }

  void removeEntity(entity_t entity) {

    bool isPlayer = entity.has_component<PlayerInfo>();
    PlayerInfo playerInfo;
    if (isPlayer) {
      playerInfo = entity.get_component<PlayerInfo>();
    }

    ServerMessage msg{MsgDeleteEntity{entity.get_component<EntityID>()}};
    broadcaster.broadcast(msg.to_message());
    entity.destroy();

    if (isPlayer) {
      removePlayer(playerInfo);
    }
  }

  std::random_device d{};
  std::default_random_engine randomEngine;

  geometry::Point randomPoint() {
    return {Scalar(randomEngine() % int(gameInfo.width())),
            Scalar(randomEngine() % int(gameInfo.height()))};
  }

  const GameInfo &getGameInfo() { return gameInfo; };

  entity_t newEntity() {
    return ecs.entityManager.create_entity(EntityID::newID());
  }

  void playerShoots(Position const &playerPosition) {
    entity_t entity = newEntity();
    Position pos;
    pos.center = playerPosition.center +
                 (1 + BULLET_RADIUS + playerShape.radius) *
                     geometry::rotate({0, -1}, playerPosition.rotation);
    pos.speed = BULLER_SPEED * unit(pos.center - playerPosition.center);
    entity.add_component<Position>(pos);
    entity.add_component<Shape>(geometry::CircleShape{BULLET_RADIUS});
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

  std::tuple<Position, Object2D> randomPosition(Shape const &shape) {
    Position pos;

    // Hope it will eventually find a good place.
    int counter = 0;
    Object2D object2d;

    vector<entity_t> collisions;
    do {
      pos.center = randomPoint();
      pos.rotation = randomEngine() % 360;
      object2d = createObject2D(pos.center, pos.rotation, shape);
      collisions = collidesWithSomething(object2d);

    } while (++counter < 50 && (!collisions.empty() ||
                                !geometry::in(object2d, gameInfo.getArea())));

    if (counter >= 50)
      throw std::runtime_error("Cannot fit object to area");

    return {pos, object2d};
  }

  // What about some 'player factory'?
  SEntity newPlayer(PlayerInfo playerInfo) {
    entity_t entity = newEntity();

    Position pos;
    geometry::Object2D object2d;
    std::tie(pos, object2d) = randomPosition(playerShape);
    entity.add_component<PlayerInfo>(playerInfo);
    entity.add_component<Position>(pos);
    entity.add_component<Shape>(playerShape);
    entity.add_component<geometry::Object2D>(std::move(object2d));
    entity.add_component<EntityMotion>();
    entity.add_component<Health>(Health{DEFAULT_HP});

    gameInfo.addPlayer(playerInfo);

    return SEntity{entity};
  }

  EntityComponentSystem &ecs;

  void playerRotatesTo(entity_t entity, geometry::Angle angle) {
    entity.sync();
    if (!entity.has_component<Position>()) {
      cout << "Player cannot move" << endl;
      return;
      // throw std::logic_error("Player is missing some components");
    }

    Position &pos = entity.get_component<Position>();
    pos.rotation = angle;
  }

  void updatePlayerMotion(PlayerAction key, EntityMotion &motion, bool press) {
    switch (key) {
    case PlayerAction::Down:
      motion.down = press;
      break;

    case PlayerAction::Up:
      motion.up = press;
      break;

    case PlayerAction::Left:
      motion.left = press;
      break;

    case PlayerAction::Right:
      motion.right = press;
      break;
    default:
      break;
    }
  }

  void playerKeyPress(entity_t entity, PlayerAction key, bool press) {
    if (!entity.sync()) {
      throw std::logic_error("Player entity sync failed");
    }

    if (!entity.has_component<Position>() ||
        !entity.has_component<EntityMotion>()) {
      cout << "Player cannot move" << endl;
      return;
      // throw std::logic_error("Player is missing some components");
    }

    Position &position = entity.get_component<Position>();
    {
      EntityMotion &playerMotion = entity.get_component<EntityMotion>();
      updatePlayerMotion(key, playerMotion, press);
      position.speed = toSpeedVector(playerMotion) * PLAYER_SPEED;
    }

    // This may invalidate previous references to components,
    // since a new bullet is created.
    if (key == PlayerAction::Fire && press) {
      playerShoots(position);
    }
  }

  void stop() { m_stop = true; }

  void main() { do_main(); }

private:
  static constexpr Scalar BULLET_RADIUS = 2.0;
  static constexpr Scalar BULLER_SPEED = 500;
  static constexpr Scalar PLAYER_SPEED = 150;
  static constexpr Scalar PLAYER_RADIUS = 20;
  geometry::CircleShape const playerShape = {PLAYER_RADIUS};

  const int DEFAULT_HP = 100;

  static geometry::Vector toSpeedVector(EntityMotion const &info) {
    Vector v{0, 0};
    if (info.down)
      v += Vector{0, +1};
    if (info.left)
      v += Vector{-1, 0};
    if (info.right)
      v += Vector{+1, 0};
    if (info.up)
      v += Vector{0, -1};
    return v;
  }

  GameInfo gameInfo{1600, 900, {}};

  // TODO we should measure the diffeence between client's and server's time.

  static auto constexpr dt = 16ms;

  void do_main() {
    m_stop = false;
    startPoint = realTime = gameTime = chrono::steady_clock::now();

    while (!m_stop) {
      realTime = chrono::steady_clock::now();

      broadcaster.iter();
      do_physics();

      gameTime = gameTime + dt;
      this_thread::sleep_until(gameTime);
    }
  }

  void do_physics() {
    using namespace std::placeholders;
    try {
      ecs.entityManager.for_each<Position, Shape, geometry::Object2D>(
          std::bind(&Impl::update_position, this, _1, _2, _3, _4));
    } catch (exception &e) {
      cerr << "An exception while updating positions" << endl;
    }
  }

  void explosiveCollision(entity_t explosive_entity, entity_t victim) {
    log() << "Explosive collision";
    if (victim.has_component<Health>()) {
      Health &h = victim.get_component<Health>();
      if (h.hp >= 10)
        h.hp -= 10;
      else
        h.hp = 0;

      updateComponent(victim, {h});

      if (h.hp <= 0) {
        // TODO remove physical components (at least position and
        victim.remove_component<Position>();
        victim.remove_component<Shape>();
        victim.remove_component<geometry::Object2D>();

        MsgRemoveComponents mrc;
        mrc.entity_id = victim.get_component<EntityID>();
        mrc.components.push_back(
            EntityComponentSystem::getComponentNumber<Position>());
        mrc.components.push_back(
            EntityComponentSystem::getComponentNumber<Shape>());
        mrc.components.push_back(
            EntityComponentSystem::getComponentNumber<geometry::Object2D>());
        broadcaster.broadcast(ServerMessage{mrc}.to_message());
      }
    }
    removeEntity(explosive_entity);
  }

  void collisionHappened(entity_t a, Position &posa, entity_t b) {
    posa.speed = geometry::Vector{0, 0};
    if (a.has_component<Explosive>())
      explosiveCollision(a, b);
    // if (b.has_component<Explosive>())
    //	explosiveCollision(b, a);
  }

  void collisionHappenedWithArea(entity_t entity, Position &pos) {
    pos.speed = geometry::Vector{0, 0};

    if (entity.has_component<Explosive>())
      removeEntity(entity);
  }

  void update_position(entity_t const &entity, Position &pos,
                       Shape const &shape, geometry::Object2D &oldObject) {
    if (!(std::fabs(pos.speed.x) < 0.01 && std::fabs(pos.speed.y) < 0.01)) {

      geometry::Point const new_center =
          pos.center + pos.speed * Scalar(dt.count() / 1000.0);
      auto currentObject2d = createObject2D(new_center, pos.rotation, shape);

      std::vector<entity_t> collisions =
          collidesWithSomething(currentObject2d, &entity);

      if (!collisions.empty()) {
        log() << "Entity " << entity.get_component<EntityID>().id()
              << " have collision now";
        for (entity_t e : collisions)
          log() << "  with: " << e.get_component<EntityID>().id();
      }

      if (!in(currentObject2d, gameInfo.getArea()))
        return collisionHappenedWithArea(entity, pos);

      if (!collisions.empty()) {
        for (entity_t e : collisions)
          collisionHappened(entity, pos, e);
        return;
      }

      pos.center = new_center;
      oldObject = currentObject2d;
    }

    // TODO possible optimization
    // We are send position every time because of rotation changes
    updateComponent(entity, {pos});
  }

  void placeRandomObstacle() {

    static std::vector<PolygonalShape> shapes = {
        PolygonalShape{{-1, 0}, {-2, -1}, {+2, -1}, {+1, 0}},
        PolygonalShape{{0, 0}, {0, 1}, {1, 0}},
        PolygonalShape{{0, 0}, {0, 1}, {1, 1}, {1, 0}},
        PolygonalShape{{0, 0}, {0, 1}, {3, 1}, {2, 0}},
        PolygonalShape{{0, 0}, {0, 1}, {1, 1}, {1, 0}},
    };

    int const which = randomEngine() % shapes.size();
    Scalar const resize = Scalar(4 + ((randomEngine() % 50) / 10.0));
    PolygonalShape shape = shapes[which];
    shape *= resize;

    Object2D obj;
    Position pos;
    std::tie(pos, obj) = randomPosition(shape);

    entity_t entity = ecs.entityManager.create_entity(EntityID::newID());
    entity.add_component<Shape>(shape);

    entity.add_component<Position>(pos);
    entity.add_component<geometry::Object2D>(obj);
  }

  void generateMap() {

    static std::vector<std::pair<PolygonalShape, Position>> obstacles = {
            /********************************/
            /*     TOP SHAPE                */
            /********************************/
            {       // long horizontal line
                    PolygonalShape{{0, 0}, {1280, 0}, // top outer line
                                   {1280, 20}, {0, 20} // top inner line
                    },
                    Position::create({150,150})
            },
            {       // left vertical line
                    PolygonalShape{{0,0},     {20,0}, // top line
                                   {20, 210}, { 0,210}// bottom line
                    },
                    Position::create({150,170})
            },
            {       // right vertical line
                    PolygonalShape{{0, 0}, {20, 0}, // top line
                                   {20, 670}, {0, 670}, // bottom line
                    },
                    Position::create({1430,80})
            },
            /*****************************************/

            {       // bottom right shape
                    PolygonalShape{{0, 0}, {20, 0}, {20, 530}, {0, 530}},
                    Position::create({1260,370})
            },

            /********************************/
            /*     PLIERS SHAPE             */
            /********************************/
            {       // body
                    PolygonalShape{{0,   0}, {200, 0}, // top line
                                   {200, 300}, {0, 400}, // bottom diagonal line
                    },
                    Position::create({960, 320})
            },
            {       // top arm upper horizontal line
                    PolygonalShape{{0,   0}, {490, 0}, // top line
                                   {490, 20}, {0, 20} // bototm line
                    },
                    Position::create({470, 320})
            },
            {       // top arm vertical line
                    PolygonalShape{{0,   0}, {20, 0}, // top line
                                   {20, 80}, {0, 80} // bottom line
                    },
                    Position::create({470, 340})
            },
            {       // bottom arm
                    PolygonalShape{{490, 20}, {0, 20}, // top line
                                   {0, 0},{490, 0} // bottom line
                    },
                    Position::create({470, 700})
            },
            /*****************************************/

            {
              PolygonalShape{{0, 180}, {260, 0}, {260, 20}, {30, 180}},
              Position::create({900,720})
            },
            {
              PolygonalShape{{100, 0}, {350, 0}, // top line
                             {160, 220}, {0, 170}}, // bottom line
              Position::create({80, 620})
            }
    };

    // A polygon
    for (size_t i = 0; i < obstacles.size(); i++) {

      PolygonalShape shape = obstacles[i].first;
      Position pos = obstacles[i].second;

      Object2D obj = createObject2D(pos.center, pos.rotation, shape);

      entity_t entity = ecs.entityManager.create_entity(EntityID::newID());
      entity.add_component<Shape>(shape);

      entity.add_component<Position>(pos);
      entity.add_component<geometry::Object2D>(obj);
    }
  }

  void updateComponent(entity_t const &entity, AnyComponent const &component) {
    MsgUpdateComponents mue;
    mue.entity_id = entity.get_component<EntityID>();
    mue.components = {component};
    ServerMessage usm{mue};
    broadcaster.broadcast(usm.to_message());
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
    cout << "[GameModel, " << (gameTime - startPoint).count() << "] ";
    return Log(cout);
  }

  IBroadcaster &broadcaster;

  std::atomic<bool> m_stop;
  std::chrono::steady_clock::time_point startPoint;
  std::chrono::steady_clock::time_point realTime;
  std::chrono::steady_clock::time_point gameTime;
};

const std::chrono::milliseconds GameModel::Impl::dt;

/*******************************************************/
/* </GameModel::Impl> <GameModel>                      */
/*******************************************************/

GameModel::GameModel(EntityComponentSystem &ecs, IBroadcaster &broadcaster) {
  pImpl = make_unique<Impl>(ecs, broadcaster);
}

GameModel::~GameModel() = default;

SEntity GameModel::newPlayer(PlayerInfo playerInfo) {
  return pImpl->newPlayer(playerInfo);
}

void GameModel::removeEntity(SEntity const &entity) {
  return pImpl->removeEntity(entity.entity);
}

void GameModel::playerKeyPress(SEntity const &entity, PlayerAction key,
                               bool press) {
  return pImpl->playerKeyPress(entity.entity, key, press);
}

void GameModel::playerRotatesTo(SEntity const &entity, geometry::Angle angle) {
  pImpl->playerRotatesTo(entity.entity, angle);
}

void GameModel::stop() { pImpl->stop(); }

void GameModel::main() { pImpl->main(); }

const GameInfo &GameModel::getGameInfo() { return pImpl->getGameInfo(); }
