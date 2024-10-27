#include "CreatureTree.hpp"
#include "creature_tree_api.h"

#ifdef __cplusplus
extern "C" {
#endif

    using namespace Creature;

    bool add_creature_to_tree
        (
        struct Thing *creature
        )
    {
        CreatureNode node;
        // Assume mappos is the center of the creature's box.
        node.bbox.min[0] = creature->mappos.x.val - ( creature->solid_size_xy / 2 );
        node.bbox.min[1] = creature->mappos.y.val - ( creature->solid_size_xy / 2 );
        node.bbox.max[0] = creature->mappos.x.val + ( creature->solid_size_xy / 2 );
        node.bbox.max[1] = creature->mappos.y.val + ( creature->solid_size_xy / 2 );
        node.index = creature->index;
        return CreatureTreeManager::getInstance()->addCreature(node);
    }

    bool clear_tree()
    {
        return CreatureTreeManager::getInstance()->clearTree();
    }

    bool nearest_creature_search
        (
        Coord2d *position,
        uint32_t radius,
        uint32_t *result_count,
        uint16_t **creature_indices
        )
    {
        if (!position || !result_count || !creature_indices)
        {
            return false;
        }

        Point<int> pos({ static_cast<int>(position->x.val), static_cast<int>(position->y.val) });
        std::vector<uint16_t> results;
        bool ok = CreatureTreeManager::getInstance()->nearestSearch(
            pos,
            radius,
            results);

        if (!ok)
        {
            return false;
        }

        uint16_t* indices = static_cast<uint16_t*>( malloc(results.size() * sizeof(uint16_t)) );
        if (indices)
        {
            for (int i = 0; i < results.size(); ++i)
            {
                indices[i] = results[i];
            }

            *creature_indices = indices;
            *result_count = results.size();
        }
        else
        {
            ok = false;
        }

        return ok;
    }

    // @todo Add function pointer support for "predicate" later.
    bool ray_creature_search
        (
        Coord2d *position,
        Coord2d *direction,
        uint32_t *result_count,
        uint16_t **creature_indices
        )
    {
        if (!position || !direction || !result_count || !creature_indices)
        {
            return false;
        }

        Point<float> pos({ static_cast<float>( position->x.val ), static_cast<float>( position->y.val ) });
        Point<float> dir({ static_cast<float>( direction->x.val ), static_cast<float>( direction->y.val ) });
        std::vector<uint16_t> results;
        bool ok = CreatureTreeManager::getInstance()->raySearch(pos, dir, results);

        if (!ok)
        {
            return false;
        }

        uint16_t* indices = static_cast<uint16_t*>( malloc(results.size() * sizeof(uint16_t)) );
        if (indices)
        {
            for (int i = 0; i < results.size(); ++i)
            {
                indices[i] = results[i];
            }

            *creature_indices = indices;
        }
        else
        {
            ok = false;
        }

        return ok;
    }


#ifdef __cplusplus
}
#endif

namespace Creature
{
    // Initialize static members
    CreatureTreeManager* CreatureTreeManager::mInstance = nullptr;
    std::mutex CreatureTreeManager::mMutex;

    CreatureTreeManager* CreatureTreeManager::getInstance()
    {
        if (mInstance == nullptr)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            if (mInstance == nullptr)
            {
                mInstance = new CreatureTreeManager();
            }
        }
        return mInstance;
    }

    CreatureTreeManager::CreatureTreeManager()
    {
    }

    bool CreatureTreeManager::addCreature(const CreatureNode& node)
    {
        mRTree.insert(node);
        return true;
    }

    bool CreatureTreeManager::clearTree()
    {
        mRTree.clear();
        return true;
    }

    bool CreatureTreeManager::nearestSearch
        (
        const Point<int>& position,
        uint32_t radius,
        std::vector<uint16_t>& results
        )
    {
        std::vector<CreatureNode> nodes;
        bool ok = mRTree.nearest(position.data, radius, std::back_inserter(nodes));

        if (ok)
        {
            for (auto node : nodes)
            {
                results.push_back(node.index);
            }
        }

        return ok;
    }

    bool CreatureTreeManager::raySearch
        (
        const Point<float>& position,
        const Point<float>& direction,
        std::vector<uint16_t>& results
        )
    {
        std::vector<CreatureNode> nodes;
        bool ok = mRTree.rayQuery(position.data, direction.data, std::back_inserter(nodes));

        if (ok)
        {
            for (auto node : nodes)
            {
                results.push_back(node.index);
            }
        }

        return ok;
    }

    size_t CreatureTreeManager::getCount()
    {
        return mRTree.count();
    }
}