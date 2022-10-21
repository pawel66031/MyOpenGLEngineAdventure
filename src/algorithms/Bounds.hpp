#ifndef BOUNDS_HPP
#define BOUNDS_HPP

#include <glm/glm.hpp>

#include "../physics/RigidBody.hpp"

enum class BoundTypes : unsigned char {
    AABB = 0x00, // Axis-aligned bounding box
    SPHERE = 0x01
};

class BoundingRegion {
public:
    BoundTypes type;

    RigidBody* instance;

    // Sphere values
    glm::vec3 center;
    float radius;

    glm::vec3 ogCenter;
    float ogRadius;

    // Bounding box values
    glm::vec3 min;
    glm::vec3 max;

    glm::vec3 ogMin;
    glm::vec3 ogMax;

    /*
        Constructors
    */

    // Initialize with type
    BoundingRegion(BoundTypes type = BoundTypes::AABB);

    // Initialize with sphere
    BoundingRegion(glm::vec3 center, float radius);

    // Initialize woth AABB
    BoundingRegion(glm::vec3 min, glm::vec3 max);

    /*
            Calculating values for the refion
    */

    // transform for instance
    void transform();

    // center
    glm::vec3 calculateCenter();

    // Calculate dimensions
    glm::vec3 calculateDimensions();

    /*
        Testing methods
    */

    // Determine if point inside
    bool containsPoint(glm::vec3 pt);

    // Determine if region inside
    bool containsRegion(BoundingRegion br);

    // Determine if region intersects (partial containment)
    bool intersectsWith(BoundingRegion br);

    // Operator overload
    bool operator==(BoundingRegion br);
};

#endif //BOUNDS_HPP