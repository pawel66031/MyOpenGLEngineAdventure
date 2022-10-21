#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "../Model.hpp"

class Sphere : public Model {
public:
    Sphere(unsigned int maxNoInstances)
        : Model("sphere", BoundTypes::SPHERE, maxNoInstances, NO_TEX | DYNAMIC) {
        
        }

    void init() {
        loadModel("../assets/models/sphere/sphere.gltf");
    }
};

#endif