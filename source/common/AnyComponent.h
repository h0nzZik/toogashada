#pragma once

#include <boost/variant/variant.hpp>

#include <common/components/Position.h>
#include <common/components/Shape.h>

// 'AnyComponent' contains the same components as 'CompList'
// except it does not contain EntityID.
// It is in 'struct' to allow forward declarations.
template <typename... Components> struct BaseAnyComponent {
  boost::variant<Components...> component;

  template <typename Archive> void serialize(Archive &archive) {
    archive(component);
  }
};
