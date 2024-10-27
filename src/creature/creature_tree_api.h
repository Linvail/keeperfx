#ifndef CREATURE_TREE_API_H
#define CREATURE_TREE_API_H

#include <stdbool.h>
#include <stdint.h>

#include "globals.h"
#include "thing_creature.h"

#ifdef __cplusplus
extern "C" {
#endif

    bool add_creature_to_tree
        (
        struct Thing *creature
        );

    bool clear_tree
        (
        );

    bool nearest_creature_search
        (
        Coord2d *position,
        uint32_t radius,
        uint32_t *result_count,
        uint16_t **creature_indices
        );

    // @todo Add function pointer support for "predicate" later.
    bool ray_creature_search
        (
        Coord2d *position,
        Coord2d *direction,
        uint32_t *result_count,
        uint16_t **creature_indices
        );

#ifdef __cplusplus
}
#endif

#endif // CREATURE_TREE_API_H