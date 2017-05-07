#pragma once

#include <boost/variant/variant.hpp>
#include <entityplus/entity.h>

#include "AnyComponent.h"
#include <common/components/EntityID.h>

using TagList = entityplus::tag_list<struct TA, struct TB>; // just an example

namespace detail {
template <typename Entity, typename AnyComponent, typename... CompList>
struct get_component_impl;

template <typename Entity, typename... Comps> struct impl_removeComponent;

template <typename Component, typename... Comps> struct impl_getComponentNumber;
}

/**
 * Provides a wrapper over EntityPlus.
 */
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

  template <typename Component> static uint32_t getComponentNumber() {
    detail::impl_getComponentNumber<Component, Components...> o{0};
    return o();
  }

  static void removeComponent(entity_t entity, uint32_t component_id) {
    detail::impl_removeComponent<entity_t, Components...>{}(entity,
                                                            component_id);
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
      if (entity.template has_component<T>()) {
        entity.template get_component<T>() = std::move(x);
      } else {
        entity.template add_component<T>(x);
      }
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

template <typename Entity> struct impl_removeComponent<Entity> {
  void operator()(Entity /*entity*/, uint32_t /*id*/) const {}
};

template <typename Entity, typename First, typename... Other>
struct impl_removeComponent<Entity, First, Other...> {
  void operator()(Entity entity, uint32_t id) const {
    if (id == 0)
      entity.template remove_component<First>();
    else
      impl_removeComponent<Entity, Other...>{}(entity, id - 1);
  }
};

template <typename Component, typename First, typename... Other>
struct impl_getComponentNumber<Component, First, Other...> {
  uint32_t n;
  impl_getComponentNumber(uint32_t n) : n(n) {}

  uint32_t operator()() const {
    if (std::is_same<Component, First>::value)
      return n;

    impl_getComponentNumber<Component, Other...> o{n + 1};
    return o();
  }
};

template <typename Component> struct impl_getComponentNumber<Component> {
  uint32_t n;
  impl_getComponentNumber(uint32_t n) : n(n) {}

  uint32_t operator()() const {
    // FIXME we should detect this on compile time
    throw std::logic_error("getComponentNumber: given type is not a component");
  }
};

} // namespace detail
