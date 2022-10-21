#include "Octree.hpp"

void Octree::calculateBounds(BoundingRegion* out, Octant octant, BoundingRegion parentRegion){
    glm::vec3 center = parentRegion.calculateCenter();
        if (octant == Octant::O1) {
            out = new BoundingRegion(center, parentRegion.max);
        }
        else if (octant == Octant::O2) {
            out = new BoundingRegion(glm::vec3(parentRegion.min.x, center.y, center.z), glm::vec3(center.x, parentRegion.max.y, parentRegion.max.z));
        }
        else if (octant == Octant::O3) {
            out = new BoundingRegion(glm::vec3(parentRegion.min.x, parentRegion.min.y, center.z), glm::vec3(center.x, center.y, parentRegion.max.z));
        }
        else if (octant == Octant::O4) {
            out = new BoundingRegion(glm::vec3(center.x, parentRegion.min.y, center.z), glm::vec3(parentRegion.max.x, center.y, parentRegion.max.z));
        }
        else if (octant == Octant::O5) {
            out = new BoundingRegion(glm::vec3(center.x, center.y, parentRegion.min.z), glm::vec3(parentRegion.max.x, parentRegion.max.y, center.z));
        }
        else if (octant == Octant::O6) {
            out = new BoundingRegion(glm::vec3(parentRegion.min.x, center.y, parentRegion.min.z), glm::vec3(center.x, parentRegion.max.y, center.z));
        }
        else if (octant == Octant::O7) {
            out = new BoundingRegion(parentRegion.min, center);
        }
        else if (octant == Octant::O8) {
            out = new BoundingRegion(glm::vec3(center.x, parentRegion.min.y, parentRegion.min.z), glm::vec3(parentRegion.max.x, center.y, center.z));
        }
}

Octree::node::node()
    : region(BoundTypes::AABB) {}

Octree::node::node(BoundingRegion bounds)
    : region(bounds) {}

Octree::node::node(BoundingRegion bounds, std::vector<BoundingRegion> objectList)
    : region(bounds) {
        objects.insert(objects.end(), objectList.begin(), objectList.end());
    }

void Octree::node::addToPending(RigidBody* instance, trie::Trie<Model*> models){
    // Get all bounding region of model
    for (BoundingRegion br : models[instance->modelId]->boundingRegions){
        br.instance = instance;
        br.transform();
        queue.push(br);
    }
}

void Octree::node::build(){
    /*
        Termination conditions
        - 1 or less objects (ie an empty leaf node)
        -dimensions are too small
    */

    // <= 1 objects
    if (objects.size() <= 1) {
        return;
    }

    // Too small
    glm::vec3 dimensions = region.calculateDimensions();
    for (int i = 0; i < 3; ++i){
        if(dimensions[i] < MIN_BOUNDS) {
            return;
        }
    }

    // Create regions
    BoundingRegion octants[NO_CHILDREN];
    for(int i = 0; i < NO_CHILDREN; ++i){
        calculateBounds(&octants[i], (Octant)(1 << i), region);
    }

    // Determine which octants to place object in
    std::vector<BoundingRegion> octLists[NO_CHILDREN]; // Array of lists of objects in each octant
    std::stack<int> delList; // List of objects that have been placed

    for(int i = 0, length = objects.size(); i < length; ++i){
        BoundingRegion br = objects[i];
        for(int j = 0; j < NO_CHILDREN; ++j){
            if(octants[j].containsRegion(br)) {
                octLists[j].push_back(br);
                delList.push(i);
                break;
            }
        }
    }

    // Remove objects on delList
    while(delList.size() != 0) {
        objects.erase(objects.begin() + delList.top());
        delList.pop();
    }

    // Populate octants
    for(int i = 0; i < NO_CHILDREN; ++i){
        if(octLists[i].size() != 0){
            children[i] = new node(octants[i], octLists[i]);
            States::activateIndex(&activeOctants, i);
            children[i]->build();
            hasChildren = true;
        }
    }

    treeBuilt = true;
    treeReady = true;
}

void Octree::node::update(){
    if (treeBuilt && treeReady) {
        // Countdown timer
        if (objects.size() == 0){
            if(!hasChildren) {
                if (currentLifespan == -1){
                    // Initial check
                    currentLifespan = maxLifespan;
                }
                else if (currentLifespan > 0){
                    // Decrement
                    --currentLifespan;
                }
            }
        }
        else {
            if (currentLifespan != -1){
                if (maxLifespan <= 64) {
                    // Extend lifespan because "hotspot"
                    maxLifespan <<= 2; 
                }
                // currentLifespan = -1;
            }
        }

        // Get moved objects that were in this leaf in previous frame
        std::stack<std::pair<int, BoundingRegion>> movedObjects;

        for (int i = 0, listSize = objects.size(); i < listSize; ++i) {
            if (States::isActive(&objects[i].instance->state, INSTANCE_MOVED)) {
                objects[i].transform();
                movedObjects.push({ i, objects[i] });
            }
        }

        // Remove dead branches
        unsigned char flags = activeOctants;
        for (int i = 0;
            flags > 0;
            flags >>= 1, ++i){
            if (States::isIndexActive(&flags, 0) && children[i]->currentLifespan == 0) {
                // Active and out of time
                if (children[i]->objects.size() > 0){
                    // Branch is dead but has children. so reset
                    children[i]->currentLifespan = -1;
                }
                else {
                    // Branch is dead
                    children[i] = nullptr;
                    States::deactivateIndex(&activeOctants, i);
                }
            }
        }

        // Update child nodes
        if(children != nullptr){
            for(unsigned char flags = activeOctants, i = 0;
                flags > 0;
                flags >>= 1, ++i) {
                if(States::isIndexActive(&flags, 0)){
                    // Active octant
                    if (children[i] != nullptr){
                        // Child not null
                        children[i]->update();
                    }
                }
            }
        }

        // Move moved objects into new nodes
        BoundingRegion movedObj;
        while (movedObjects.size() != 0){
            /*
                for each movec object
                - transverse up tree (start with current mode) until find a node that completely enclose the object
                - call insert (push object as far down as possible)
            */
        
            movedObj = movedObjects.top().second; // Set to top object in stack
            node* current = this;

            while(!current->region.containsRegion(movedObj)){
                if(current->parent != nullptr) {
                    current = current->parent;
                }
                else {
                    break; // If root node, the leave
                }
            }

            /*
                Once finished
                - remove from objects list
                - remove from movedObjects stack
                - insert into found region
            */
            objects.erase(objects.begin() + movedObjects.top().first);
            movedObjects.pop();
            current->insert(movedObj);

            // Collision detection
            // TODO
        }

    }
    else {
        // Process pending results
        if (queue.size() > 0){
            processPending();
        }
    }
}

void Octree::node::processPending(){
    if (!treeBuilt){
        // Add objects to be sorted into branches when built
        while (queue.size() != 0){
            objects.push_back(queue.front());
            queue.pop();
        }
        build();
    }
    else {
        // Insert the objects immediately
        while(queue.size() != 0){
            insert(queue.front());
            queue.pop();
        }
    }
}

bool Octree::node::insert(BoundingRegion obj){
    /*
        Termination conditions
        - no objects (an empty leaf node)
        - dimensions are less than MIN_BOUNDS
    */

    glm::vec3 dimensions = region.calculateDimensions();
    if (objects.size() == 0 || 
            dimensions.x < MIN_BOUNDS ||
            dimensions.y < MIN_BOUNDS ||
            dimensions.z < MIN_BOUNDS
    ){
            objects.push_back(obj);
            return true;
    }

    // Safe guard if object doesn't fit
    if (!region.containsRegion(obj)) {
        return parent == nullptr ? false : parent->insert(obj);
    }

    // Create regions if not defined
    BoundingRegion octants[NO_CHILDREN];
    for(int i = 0; i < NO_CHILDREN; ++i) {
        if(children[i] != nullptr) {
            octants[i] = children[i]->region;
        }
        else {
            calculateBounds(&octants[i], (Octant)(1 << i), region);
        }
    }

    // Find region that fits item entirely
    for(int i = 0; i < NO_CHILDREN; ++i){
        if (octants[i].containsRegion(obj)){
            if (children[i] != nullptr){
                return children[i]->insert(obj);
            }
            else {
                // Create node for child
                children[i] = new node(octants[i], { obj });
                States::activateIndex(&activeOctants, i);
                return true;
            }
        }
    }
    // Doesn't fit into children
    objects.push_back(obj);
    return true;
}

void Octree::node::destroy(){
    // Clearing out children
    if(children != nullptr) {
        for(int flags = activeOctants, i = 0;
            flags > 0;
            flags >> 1, ++i){
                if (States::isActive(&flags, 0)){
                    // Active
                    if (children[i] != nullptr){
                        children[i]->destroy();
                        children[i] = nullptr;
                    }
                }
            }
    }

    // Clear this node
    objects.clear();
    while (queue.size() != 0){
        queue.pop();
    }
}