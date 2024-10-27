#include <doctest/doctest.h>

#include <iostream>
#include <numeric>
#include <sstream>

#include "creature/creature_tree_api.h"
#include "creature/CreatureTree.hpp"

typedef struct Thing Thing;

Thing* make_creature(uint16_t index, uint32_t x, uint32_t y, uint16_t size)
{
    Thing* obj = new Thing();

    obj->index = index;
    obj->mappos.x.val = x;
    obj->mappos.y.val = y;
    obj->solid_size_xy = size;

    return obj;
}

void print_creature(Thing* obj)
{
    MESSAGE("Creature: index=" << obj->index << ", x=" << obj->mappos.x.val << ", y=" <<
        obj->mappos.x.val << ", xy size=" << obj->solid_size_xy << ".");
}

TEST_CASE("Test nearest search for creature")
{
    MESSAGE("Creating creatures:\n");
    Thing* creature1 = make_creature(100, 5000, 5000, 300);
    print_creature(creature1);
    Thing* creature2 = make_creature(101, 5000, 8000, 300);
    print_creature(creature2);

    bool ok = add_creature_to_tree(creature1);
    CHECK(ok);
    ok = add_creature_to_tree(creature2);
    CHECK(ok);

    // Test if the count is correct.
    using namespace Creature;
    CHECK(CreatureTreeManager::getInstance()->getCount() == 2u);
    // Test if clearTree() works.
    CreatureTreeManager::getInstance()->clearTree();
    CHECK(CreatureTreeManager::getInstance()->getCount() == 0u);

    // Add them back.
    ok = add_creature_to_tree(creature1);
    CHECK(ok);
    ok = add_creature_to_tree(creature2);
    CHECK(ok);

    Thing* creature3 = make_creature(102, 5000, 8000 + 150 + 1, 300); // not in 3000 radius.
    print_creature(creature3);
    ok = add_creature_to_tree(creature3);
    CHECK(ok);

    Thing* creature4 = make_creature(103, 2000, 5000, 300);
    print_creature(creature4);
    ok = add_creature_to_tree(creature4);
    CHECK(ok);

    Thing* creature5 = make_creature(104, 8000, 5000, 300);
    print_creature(creature5);
    ok = add_creature_to_tree(creature5);
    CHECK(ok);

    Thing* creature6 = make_creature(105, 1000, 5000, 300); // not in 3000 radius.
    print_creature(creature6);
    ok = add_creature_to_tree(creature6);
    CHECK(ok);

    Thing* creature7 = make_creature(106, 8000 + 150 + 1, 5000, 300); // not in 3000 radius.
    print_creature(creature7);
    ok = add_creature_to_tree(creature7);
    CHECK(ok);

    Thing* creature8 = make_creature(107, 5000, 2000, 300);
    print_creature(creature8);
    ok = add_creature_to_tree(creature8);
    CHECK(ok);

    Thing* creature9 = make_creature(108, 7121, 7121, 300);
    print_creature(creature9);
    ok = add_creature_to_tree(creature9);
    CHECK(ok);

    Thing* creature10 = make_creature(109, 7121 + 150 + 1, 7121 + 150 + 1, 300); // not in 3000 radius.
    print_creature(creature10);
    ok = add_creature_to_tree(creature10);
    CHECK(ok);

    Coord2d pos;
    pos.x.val = creature1->mappos.x.val;
    pos.y.val = creature1->mappos.y.val;
    uint32_t result_count = 0;
    uint16_t* result_indices = nullptr;
    MESSAGE("Do nearest search on the creature1 with radius=" << 3000 <<  ".");
    ok = nearest_creature_search(&pos, 3000, &result_count, &result_indices);
    CHECK(ok);

    MESSAGE("Result of nearest search: \n");
    for (int i = 0; i < result_count; ++i)
    {
        MESSAGE("Creature index=" << result_indices[i] <<  ".");
        CHECK(result_indices[i] != creature3->index);
        CHECK(result_indices[i] != creature6->index);
        CHECK(result_indices[i] != creature7->index);
        CHECK(result_indices[i] != creature10->index);
    }

    delete creature1;
    delete creature2;
    delete creature3;
    delete creature4;
    delete creature5;
    delete creature6;
    delete creature7;
    delete creature8;
    delete creature9;
    delete creature10;
}