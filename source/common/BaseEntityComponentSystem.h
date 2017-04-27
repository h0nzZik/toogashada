#pragma once

#include <boost/variant/variant.hpp>
#include <entityplus/entity.h>

#include "AnyComponent.h"

using TagList = entityplus::tag_list<struct TA, struct TB>; // just an example

namespace detail {
template < typename Entity, typename AnyComponent, typename ... CompList > struct add_component_impl;
}

template < typename... Components>
class BaseEntityComponentSystem {
public:
	using CompList = entityplus::component_list<EntityID, Components...>;
	using EntityManager = entityplus::entity_manager<CompList, TagList>;

	EntityManager entityManager;
	using entity_t = typename EntityManager::entity_t;

	using AnyComponent = BaseAnyComponent<Components...>;

	static std::vector<AnyComponent> all_components(entity_t entity) {
		std::vector<AnyComponent> v;
		detail::add_component_impl<entity_t, AnyComponent, Components...>::add_component(v, entity);
#if 0
		if (entity.template has_component<Shape>())
			v.push_back({entity.template get_component<Shape>()});
		if (entity.template has_component<Position>())
			v.push_back({entity.template get_component<Position>()});
#endif
		return v;
	}

};

namespace detail {
template <typename Entity, typename AnyComponent> struct add_component_impl<Entity, AnyComponent> {
	static void add_component(std::vector<AnyComponent> & /*v*/, Entity /*entity*/) {

	}
};

template < typename Entity, typename AnyComponent, typename Comp, typename ... CompList >
struct add_component_impl<Entity, AnyComponent, Comp, CompList...> {
	static void add_component(std::vector<AnyComponent> & v, Entity entity) {
		if (entity.template has_component<Comp>())
			v.push_back({entity.template get_component<Comp>()});
		add_component_impl<Entity, AnyComponent, CompList...>::add_component(v, entity);
	}
};

}
