#include <iostream>
#include <entityplus/entity.h>

#include <boost/variant/get.hpp>
#include <common/EntityComponentSystem.h>

#include "doctest.h"

// Pripadne abychom je synchronizovali.
TEST_CASE("Entity component system") {
	using ECS = BaseEntityComponentSystem<int, float>;
	using Entity = ECS::entity_t;
	ECS ecs;
	Entity e = ecs.entityManager.create_entity(int(5));
	e.add_component<float>(4.2f);
	CHECK(e.get_component<int>() == 5);
	CHECK(e.get_component<float>() == 4.2f);

	// Copy all components
	auto all = ECS::all_components(e);
	CHECK(all.size() == 2);
	Entity e2 = ecs.entityManager.create_entity();
	ECS::add_component(e2, all[0]);
	ECS::add_component(e2, all[1]);
	CHECK(e2.get_status() == entityplus::entity_status::OK);
	CHECK(e2.get_component<int>() == 5);
	CHECK(e2.get_component<float>() == 4.2f);

	// Modify one of them and update it back
	e2.get_component<float>() = 3.14f;
	ECS::update_components(e, ECS::all_components(e2));
	CHECK(e.get_component<float>() == 3.14f);
}

TEST_CASE("Entities: add existing component") {
	using ECS = BaseEntityComponentSystem<int, float>;
	using Entity = ECS::entity_t;
	ECS ecs;
	Entity e = ecs.entityManager.create_entity(int(5));
#if 0
	e.add_component<int>(6);
	e.sync();
#else
	e.get_component<int>() = 6;
#endif
	CHECK(e.get_component<int>() == 6);

}
