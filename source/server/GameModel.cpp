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
#include "common/GameInfo.h"

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
    }

    ~Impl() {
        m_stop = true;
    }

    geometry::CircleShape const playerShape = {10};

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
                });

        return colliding;
    }

    void removePlayer(const PlayerInfo &info) {

        gameInfo.removePlayer(info);
    }

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
    std::default_random_engine randomEngine{d()};

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

	// What about some 'player factory'?
    SEntity newPlayer(PlayerInfo playerInfo) {
        entity_t entity = newEntity();
        Position pos;

        // Hope it will eventually find a good place.
        int counter = 0;
        geometry::Object2D object2d;

        // TODO refacto this to separate function
        vector<entity_t> collisions;
        do {
            pos.center = randomPoint();
            object2d = createObject2D(pos.center, 0, playerShape);
            collisions = collidesWithSomething(object2d);

        } while (++counter < 50 &&
                 (!collisions.empty() ||
                  !geometry::in(pos.center, playerShape, gameInfo.getArea())));

        if (counter >= 50)
            throw std::runtime_error("Cannot fit player to area");

        log() << "Found place for player on " << counter << "th try";

        playerInfo.hp = DEFAULT_HP;
        entity.add_component<PlayerInfo>(playerInfo);
        entity.add_component<Position>(pos);
        entity.add_component<Shape>(playerShape);
        entity.add_component<geometry::Object2D>(std::move(object2d));
        entity.add_component<EntityMotion>();
		entity.add_component<Health>(Health{80});

        gameInfo.addPlayer(playerInfo);

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
            !entity.has_component<EntityMotion>()) {
            throw std::logic_error("Player is missing some components");
        }

        Position &position = entity.get_component<Position>();
        EntityMotion &playerMotion = entity.get_component<EntityMotion>();
        switch (key) {
            case PlayerAction::Down:
                playerMotion.down = press;
                break;

            case PlayerAction::Up:
                playerMotion.up = press;
                break;

            case PlayerAction::Left:
                playerMotion.left = press;
                break;

            case PlayerAction::Right:
                playerMotion.right = press;
                break;

            case PlayerAction::Fire:
                if (press)
                    playerShoots(position);
                break;

            default:
                throw std::runtime_error("Unknown player action");
        }
        position.speed = toSpeedVector(playerMotion);
    }

    void stop() {
        m_stop = true;
    }

    void main() {
        do_main();
    }

private:
    static constexpr Scalar bullet_r = 1.5;
    static constexpr Scalar player_r = 10;
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
        return v * Scalar(15);
    }


//    geometry::RectangularArea const game_area{{0,   0},
//                                              {100, 100}};
    GameInfo gameInfo {100, 100, {}};

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
        ecs.entityManager.for_each<Position, Shape, geometry::Object2D>(
                std::bind(&Impl::update_position, this, _1, _2, _3, _4));
    }

    void explosiveCollision(entity_t explosive_entity, entity_t victim) {
        log() << "Explosive collision";
		if (victim.has_component<Health>()) {
			Health & h = victim.get_component<Health>();
			if (h.hp >= 10)
				h.hp -= 10;
			else
				h.hp = 0;
			broadcaster.updateEntity(victim, {h});
		}
        removeEntity(explosive_entity);
    }

    void collisionHappened(entity_t a, Position &posa, entity_t b) {
        posa.speed = geometry::Vector{0, 0};
        if (a.has_component<Explosive>())
            explosiveCollision(a, b);
        if (b.has_component<Explosive>())
        	explosiveCollision(b, a);
    }

    void collisionHappenedWithArea(entity_t entity, Position &pos) {
        pos.speed = geometry::Vector{0, 0};

        if (entity.has_component<Explosive>())
			removeEntity(entity);
    }

    void update_position(entity_t const &entity, Position &pos,
                         Shape const &shape, geometry::Object2D &oldObject) {
        if (std::fabs(pos.speed.x) < 0.01 && std::fabs(pos.speed.y) < 0.01)
            return;

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

        if (!in(new_center, shape, gameInfo.getArea()))
            return collisionHappenedWithArea(entity, pos);

        if (!collisions.empty()) {
            for (entity_t e : collisions)
                collisionHappened(entity, pos, e);
            return;
        }

        pos.center = new_center;
        oldObject = currentObject2d;
        cout << pos.rotation << endl;
        broadcaster.updateEntity(entity, {pos});
    }

    void generateMap() {
        // A polygon
        entity_t entity = ecs.entityManager.create_entity(EntityID::newID());
        entity.add_component<Shape>(
                geometry::PolygonalShape{{-10, 0},
                                         {-20, -10},
                                         {+20, -10},
                                         {+10, 0}});
        Position pos;
        pos.speed = {0, 0};
        pos.center = geometry::Point{30, 30};
        pos.angularSpeed = 0;
        pos.rotation = 0;
        entity.add_component<Position>(pos);
        entity.add_component<geometry::Object2D>(
                geometry::createObject2D(pos.center, 0, entity.get_component<Shape>()));

    }

    class Log {
        ostream &o;

    public:
        explicit Log(ostream &o) : o(o) {}

        ~Log() { o << endl; }

        operator ostream &() { return o; }

        template<typename T>
        ostream &operator<<(T const &x) {
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

GameModel::GameModel(EntityComponentSystem &ecs, IBroadcaster &broadcaster){
    pImpl = make_unique<Impl>(ecs, broadcaster);
}

GameModel::~GameModel() = default;

SEntity GameModel::newPlayer(PlayerInfo playerInfo) { return pImpl->newPlayer(playerInfo); }

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

void GameModel::stop() {
    pImpl->stop();
}

void GameModel::main() {
    pImpl->main();
}

const GameInfo &GameModel::getGameInfo() {
    return pImpl->getGameInfo();
}
