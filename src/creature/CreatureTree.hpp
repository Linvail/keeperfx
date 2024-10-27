#ifndef CREATURE_TREE_HPP
#define CREATURE_TREE_HPP

#include <THST/RTree.h>
#include <mutex>

namespace Creature
{
	template <typename T> struct Point
	{
		union
		{
			T data[2];
			struct
			{
				T x, y;
			};
		};

		void set(T x, T y)
		{
			this->x = x;
			this->y = y;
		}
	};

    struct CreatureNode
    {
		spatial::BoundingBox<int, 2> bbox;
		uint16_t index;

		bool operator==(const CreatureNode& other) const
		{
			return index == other.index;
		}
    };

	// use a custom indexable for custom objects
	struct Indexable
	{
		const int* min(const CreatureNode& value) const { return value.bbox.min; }
		const int* max(const CreatureNode& value) const { return value.bbox.max; }
	};

	class CreatureTreeManager
	{
	public:
        CreatureTreeManager(const CreatureTreeManager& obj) = delete;
        static CreatureTreeManager* getInstance();

		bool addCreature
			(
			const CreatureNode& node
			);

		bool clearTree();

		bool nearestSearch
			(
			const Point<int>& position,
			uint32_t radius,
			std::vector<uint16_t>& results
			);

		bool raySearch
			(
			const Point<float>& position,
			const Point<float>& direction,
			std::vector<uint16_t>& results
			);

		size_t getCount();

    private:
        CreatureTreeManager();

    private:
        static CreatureTreeManager* mInstance;
        static std::mutex mMutex;
		spatial::RTree<int, CreatureNode, 2, 4, 2, Indexable> mRTree;
	};

}

#endif // CREATURE_TREE_HPP