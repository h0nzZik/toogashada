#pragma once

#include <boost/variant/variant.hpp>
#include <entityplus/entity.h>

#include "AnyComponent.h"
#include <common/components/EntityID.h>

using TagList = entityplus::tag_list<struct TA, struct TB>; // just an example

namespace detail {
template <typename Entity, typename AnyComponent, typename... CompList>
struct get_component_impl;
}

template <typename... Components> class BaseEntityComponentSystem {
public:
  using CompList = entityplus::component_list<EntityID, Components...>;
  using EntityManager = entityplus::entity_manager<CompList, TagList>;

  EntityManager entityManager;
  using entity_t = typename EntityManager::entity_t;

  using AnyComponent = BaseAnyComponent<Components...>;

  static std::vector<AnyComponent> all_components(entity_t &entity) {
    std::vector<AnyComponent> v;
    detail::get_component_impl<entity_t, AnyComponent,
                               Components...>::get_component(v, entity);
    return v;
  }

private:
  struct adder : public boost::static_visitor<void> {
    entity_t &entity;
    explicit adder(entity_t &entity) : entity(entity) {}
    template <typename T> void operator()(T x) {
      entity.template add_component<T>(x);
    }
  };

  struct updater : public boost::static_visitor<void> {
    entity_t &entity;
    explicit updater(entity_t &entity) : entity(entity) {}
    template <typename T> void operator()(T x) {
      entity.template get_component<T>() = std::move(x);
      entity.template add_component<T>(x);
    }
  };

public:
  static void add_component(entity_t &entity, AnyComponent const &any) {
    adder v{entity};
    boost::apply_visitor(v, any.component);
  }

  static void add_components(entity_t &entity,
                             std::vector<AnyComponent> const &components) {
    for (AnyComponent const &component : components) {
      add_component(entity, component);
    }
  }

  static void update_component(entity_t &entity, AnyComponent const &any) {
    updater v{entity};
    boost::apply_visitor(v, any.component);
  }

  static void update_components(entity_t &entity,
                                std::vector<AnyComponent> const &components) {
    for (AnyComponent const &component : components) {
      update_component(entity, component);
    }
  }
};

namespace detail {
template <typename Entity, typename AnyComponent>
struct get_component_impl<Entity, AnyComponent> {
  static void get_component(std::vector<AnyComponent> & /*v*/,
                            Entity & /*entity*/) {}
};

template <typename Entity, typename AnyComponent, typename Comp,
          typename... CompList>
struct get_component_impl<Entity, AnyComponent, Comp, CompList...> {
  static void get_component(std::vector<AnyComponent> &v, Entity &entity) {
    if (entity.template has_component<Comp>())
      v.push_back({entity.template get_component<Comp>()});
    get_component_impl<Entity, AnyComponent, CompList...>::get_component(
        v, entity);
  }
};
}
