#include <iostream>
#include <entityplus/entity.h>

#include <common/EntityComponentSystem.h>

#include "doctest.h"

using namespace std;

TEST_CASE("Entity component system") {
	struct S1 {int x;};
	struct S2 {float y;};
	using ECS = BaseEntityComponentSystem<S1, S2>;
	using Entity = ECS::entity_t;
	ECS ecs;
	Entity e = ecs.entityManager.create_entity(S1{5});
	e.add_component<S2>({4.2});

	auto all = ECS::all_components(e);
	CHECK(all.size() == 2);
}


