#pragma once

#include <common/EntityComponentSystem.h>

/**
 * Wrapper for entity_t, because that is an alias.
 */
struct SEntity {
  entity_t entity;
};
