#ifndef OCTREE_HPP
#define OCTREE_HPP

#define NO_CHILDREN 8
#define MIN_BOUNDS 0.5

#include <vector>
#include <queue>
#include <stack>

#include "List.hpp"
#include "States.hpp"
#include "Bounds.hpp"
#include "Trie.hpp"

#include "../graphics/Model.hpp"

namespace Octree {
    enum class Octant : unsigned char {
        O1 = 0x01,
        O2 = 0x02,
        O3 = 0x04,
        O4 = 0x08,
        O5 = 0x10,
        O6 = 0x20,
        O7 = 0x40,
        O8 = 0x80
    };

    /*
        Utility methods callbacks
    */

   //calculate bounds of specified quadrant in bounding region
    void calculateBounds(BoundingRegion* out, Octant octant, BoundingRegion parentRegion);

    class node {
    public:
        node* parent;

        node* children[NO_CHILDREN];

        unsigned char activeOctants;

        bool hasChildren = false;

        bool treeReady = false;
        bool treeBuilt = false;

        short maxLifespan = 8;
        short currentLifespan = -1;

        std::vector<BoundingRegion> objects;
        std::queue<BoundingRegion> queue;

        BoundingRegion region;

        node();

        node(BoundingRegion bounds);

        node(BoundingRegion bounds, std::vector<BoundingRegion> objectList);

        void addToPending(RigidBody* instance, trie::Trie<Model*> models);

        void build();

        void update();

        void processPending();

        bool insert(BoundingRegion obj);

        void destroy();
    };
}

#endif //OCTREE_HPP