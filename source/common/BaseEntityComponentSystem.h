#pragma once

#include <boost/variant/variant.hpp>
#include <entityplus/entity.h>

#include "AnyComponent.h"

using TagList = entityplus::tag_list<struct TA, struct TB>; // just an example

namespace detail {
template < typename Entity, typename AnyComponent, typename ... CompList >
struct get_component_impl;
}

template < typename... Components>
class BaseEntityComponentSystem {
public:
	using CompList = entityplus::component_list<EntityID, Components...>;
	using EntityManager = entityplus::entity_manager<CompList, TagList>;

	EntityManager entityManager;
	using entity_t = typename EntityManager::entity_t;

	using AnyComponent = BaseAnyComponent<Components...>;

	static std::vector<AnyComponent> all_components(entity_t & entity) {
		std::vector<AnyComponent> v;
		detail::get_component_impl<
			entity_t,
			AnyComponent,
			Components...
		>::get_component(v, entity);
		return v;
	}

private:
	struct vis : public boost::static_visitor<void> {
		entity_t & entity;
		explicit vis(entity_t & entity) : entity(entity) {}
		template < typename T >
		void operator()(T x) {
			entity.template add_component<T>(x);
		}
	};
public:
	static void add_component(entity_t & entity, AnyComponent const & any) {
		vis v{entity};
		boost::apply_visitor(v, any.component);
	}
};

namespace detail {
template <typename Entity, typename AnyComponent>
struct get_component_impl<Entity, AnyComponent> {
	static void get_component(std::vector<AnyComponent> & /*v*/, Entity & /*entity*/) {

	}
};

template < typename Entity, typename AnyComponent, typename Comp, typename ... CompList >
struct get_component_impl<Entity, AnyComponent, Comp, CompList...> {
	static void get_component(std::vector<AnyComponent> & v, Entity & entity) {
		if (entity.template has_component<Comp>())
			v.push_back({entity.template get_component<Comp>()});
		get_component_impl<Entity, AnyComponent, CompList...>::get_component(v, entity);
	}
};

}
