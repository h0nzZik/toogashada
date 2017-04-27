#include <iostream>
#include <entityplus/entity.h>

#include <boost/variant/get.hpp>
#include <common/EntityComponentSystem.h>

#include "doctest.h"

// TODO davat si pozor na to, abychom nemodifikovali kopie entit.
// Pripadne abychom je synchronizovali.
TEST_CASE("Entity component system") {
	using ECS = BaseEntityComponentSystem<int, float>;
	using Entity = ECS::entity_t;
	ECS ecs;
	Entity e = ecs.entityManager.create_entity(int(5));
	e.add_component<float>(4.2f);
	CHECK(e.get_component<int>() == 5);
	CHECK(e.get_component<float>() == 4.2f);

	auto all = ECS::all_components(e);
	CHECK(all.size() == 2);
	Entity e2 = ecs.entityManager.create_entity();
	ECS::add_component(e2, all[0]);
	ECS::add_component(e2, all[1]);
	CHECK(e2.get_status() == entityplus::entity_status::OK);
	CHECK(e2.get_component<int>() == 5);
	CHECK(e2.get_component<float>() == 4.2f);
}
