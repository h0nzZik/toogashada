// Standard
#include <chrono>
#include <iostream>
#include <ctime>
#include <thread>
#include <atomic>

#include <common/Geometry.h>
#include <common/EntityComponentSystem.h>
#include <common/geometry/collision.h>
#include <common/geometry/Object2D.h>

// Server
#include "IBroadcaster.h"
#include "SEntity.h"

// Self
#include "GameModel.h"

using namespace std;
using namespace std::chrono_literals;

class GameModel::Impl {
public:
	Impl(EntityComponentSystem & ecs, IBroadcaster & broadcaster) :
		ecs{ecs},
		broadcaster{broadcaster},
		runner{&Impl::main, this}
	{ }

	~Impl() {
		stop = true;
		runner.join();
	}

	geometry::PolygonalShape const playerShape = {
			{-10, 0},
			{-20, -10},
			{+20, -10},
			{+10, 0}
	};

	SEntity newPlayer() {
		entity_t entity = ecs.entityManager.create_entity(EntityID::newID());
		Position pos;

		// Hope it will eventually find a good place.
		// TODO collision with other players
		// TODO counter. We might reject the player.
		int counter = 0;
		bool have_collision;
		do{
			pos.center = {
					Scalar(rand() % int(game_area.bottomRight().x)),
					Scalar(rand() % int(game_area.bottomRight().y))
			};

			auto newPlayerObject2d = createObject2D(pos.center, 0, playerShape);

			have_collision = false;
			ecs.entityManager.for_each<Position, Shape>([&](auto, Position const &p, Shape const &shape){
				auto oldObject2d = createObject2D(p.center, 0, shape);
				have_collision = have_collision || geometry::collision(newPlayerObject2d, oldObject2d);
			});
		}while(++counter < 50 && (have_collision || !geometry::in(pos.center, playerShape, game_area)));

		if (counter >= 50)
			throw std::runtime_error("Cannot fit player to area");

		cout << "Found place for player on " << counter << "th try" << endl;

		entity.add_component<Position>(pos);
		//entity.add_component<Shape>(CircleShape{5.1});
		entity.add_component<Shape>(playerShape);

		return SEntity{entity};
	}

	EntityComponentSystem & ecs;
private:

	geometry::RectangularArea const game_area{{0, 0}, {100, 100}};
	// TODO we should measure the diffeence between client's and server's time.

	static auto constexpr dt = 16ms;
	void main() {
		stop = false;
		startPoint = realTime = gameTime = chrono::steady_clock::now();

		while(!stop) {
			realTime = chrono::steady_clock::now();

			do_physics();

			gameTime = gameTime + dt;
			this_thread::sleep_until(gameTime);
		}
	}

	void do_physics() {
		using namespace std::placeholders;
		ecs.entityManager.for_each<Position, Shape>(std::bind(&Impl::update_position, this, _1, _2, _3));
	}

	// TODO: make less computations. We may store the final objects somewhere.
	// FIXME they freeze after first collision
	void update_position(entity_t const & entity, Position & pos, Shape const &shape) {
		if (pos.speed == geometry::Vector{0, 0})
			return;

		geometry::Point const new_center = pos.center + pos.speed * Scalar(dt.count() / 1000.0);
		auto currentObject2d = createObject2D(pos.center, 0, shape);
		bool have_collision = false;
		ecs.entityManager.for_each<Position, Shape>([&](entity_t curr_entity, Position const &p, Shape const &shape){
			if (curr_entity == entity)
				return;
			auto oldObject2d = createObject2D(p.center, 0, shape);
			have_collision = have_collision || geometry::collision(currentObject2d, oldObject2d);
		});

		if (have_collision || !in(new_center, shape, game_area)) {
			pos.speed = -pos.speed;
		} else {
			pos.center = new_center;
		}

		broadcaster.updateEntity(entity, {pos});
	}

	IBroadcaster & broadcaster;

	std::thread runner;
	std::atomic<bool> stop;
	std::chrono::steady_clock::time_point startPoint;
	std::chrono::steady_clock::time_point realTime;
	std::chrono::steady_clock::time_point gameTime;
};

const std::chrono::milliseconds GameModel::Impl::dt;

GameModel::GameModel(EntityComponentSystem & ecs, IBroadcaster & broadcaster) :
		ecs(ecs)
{
	pImpl = make_unique<Impl>(ecs, broadcaster);
}

GameModel::~GameModel() = default;

SEntity GameModel::newPlayer() {
	return pImpl->newPlayer();
}
