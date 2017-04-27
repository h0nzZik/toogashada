#pragma once

#include <entityplus/entity.h>

#include <common/components/EntityID.h>
#include <common/components/Shape.h>
#include <common/components/Position.h>

// Just an example

struct PlayerIdentity {
    std::string name;
};


#if 0
class health {
    int health_, maxHealth_;
public:
    health(int health, int maxHealth) :
        health_(health), maxHealth_(maxHealth){}

#if 0
    int addHealth(int health) {
        return health_ = std::max(health+health_, maxHealth);
    }
#endif
};
#endif

// TODO muzeme pouzivat i udalosti a pomoci nich si synchronizovat vzdalene entity managery...
// (mozna).

// 'AnyComponent' contains the same components as 'CompList'
// except it does not contain EntityID.
// It is in 'struct' to allow forward declarations.
struct AnyComponent {
	boost::variant<Shape, Position> component;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(component);
	}
};

using CompList = entityplus::component_list<EntityID, Shape, Position>;
using TagList = entityplus::tag_list<struct TA, struct TB>; // just an example
using EntityManager = entityplus::entity_manager<CompList, TagList>;
using entity_t = typename EntityManager::entity_t;

class EntityComponentSystem {
public:
	EntityManager entityManager;

	static std::vector<AnyComponent> all_components(entity_t entity) {
		std::vector<AnyComponent> v;
		if (entity.has_component<Shape>())
			v.push_back({entity.get_component<Shape>()});
		if (entity.has_component<Position>())
			v.push_back({entity.get_component<Position>()});
		return {};
	}
};
