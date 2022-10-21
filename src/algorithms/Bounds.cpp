#include "Bounds.hpp"

/*
    Constructors
*/

// Initialize with type
BoundingRegion::BoundingRegion(BoundTypes type) 
    : type(type) {}

// Initialize with sphere
BoundingRegion::BoundingRegion(glm::vec3 center, float radius) 
    : type(BoundTypes::SPHERE), center(center), ogCenter(center), radius(radius), ogRadius(radius) {

}

// Initialize woth AABB
BoundingRegion::BoundingRegion(glm::vec3 min, glm::vec3 max)
    : type(BoundTypes::AABB), min(min), ogMin(min), max(max), ogMax(max) {

}

/*
        Calculating values for the refion
*/

void BoundingRegion::transform(){
    if(instance){
        if (type == BoundTypes::AABB){
            min = ogMin * instance->size + instance->pos;
            max = ogMax * instance->size + instance->pos;
        }
        else{
            center = ogCenter * instance->size + instance->pos;
            radius = ogRadius * instance->size.x;
        }
    }
}

// center
glm::vec3 BoundingRegion::calculateCenter(){
    return( type == BoundTypes::AABB) ? (min + max) / 2.0f : center;
}

// Calculate dimensions
glm::vec3 BoundingRegion::calculateDimensions(){
    return (type == BoundTypes::AABB) ? (max - min) : glm::vec3(2.0f * radius);
}

/*
    Testing methods
*/

// Determine if point inside
bool BoundingRegion::containsPoint(glm::vec3 pt){
    if (type == BoundTypes::AABB){
        // box = point must be larger than man and smaller than max
        return (pt.x >= min.x) && (pt.x <= max.x) &&
            (pt.x >= min.y) && (pt.x <= max.y) &&
            (pt.x >= min.z) && (pt.x <= max.z);
    } else {
        // Sphere - distance must be less than radius
        // x^2 + y^2 + z^2 <= r^2
        float distSquared = 0.0f;
        for(int i = 0; i < 3; ++i){
            distSquared += (center[i] - pt[i]) * (center[i] - pt[i]); // cubed values
        }
        return distSquared <= (radius * radius);
    }
}

// Determine if region completely inside
bool BoundingRegion::containsRegion(BoundingRegion br){
    if (br.type == BoundTypes::AABB) {
        // if br is a box, just has to contain min and max
        return containsPoint(br.min) && containsPoint(br.max);
    }
    else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE) {
        // if both spheres, combination of distance from centers and br.radius is less than radius
        return glm::length(center - br.center) + br.radius < radius;
    }
    else {
        // if this is a box and br is a sphere
        if(!containsPoint(br.center)) {
            // center is outside of the box
            return false;
        }

        // center inside the boc
        /*
            for each axis (x, y, z)
            - if distance to each side is smaller than the radius, return false
        */
       for(int i = 0; i < 3; ++i){
            if(abs(max[i] - br.center[i]) < br.radius ||
               abs(br.center[i] - min[i]) < br.radius) {
                return false;
            }
       }
       return true;
    }
}

// Determine if region intersects (partial containment)
bool BoundingRegion::intersectsWith(BoundingRegion br){
    // Overlap on all axes
    
    if (type == BoundTypes::AABB && br.type == BoundTypes::AABB) {
        // Both boxes

        glm::vec3 rad = calculateDimensions() / 2.0f;       // "radius" of this box
        glm::vec3 radBr = br.calculateDimensions() / 2.0f;  // "radius" of br

        glm::vec3 center = calculateCenter();               // center of this box
        glm::vec3 centerBr = br.calculateCenter();               // center of br

        glm::vec3 dist = abs(center - centerBr);

        for (int i = 0; i < 3; ++i){
            if(dist[i] > rad[i] + radBr[i]) {
                // No overlap on this axis
                return false;
            }
        }

        // failed to prove wrong on each axis
        return true;
    }
    else if(type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE) {
        // both spheres - distance between centers must be less than combined radius

        return glm::length(center - br.center) < (radius + br.radius);
    }
    else if(type == BoundTypes::SPHERE) {
        // this is a sphere, br is a box

        // determine if sphere is above top, below bottom etc
        // find distance (squared) to the closest plane
        float distSquared = 0.0f;
        for (int i = 0; i < 3; ++i) {
            // determine closest side
            float closestPt = std::max(br.min[i], std::min(center[i], br.max[i]));
            // add distance
            distSquared += (closestPt - center[i]) * (closestPt - center[i]);
        }
 
        return distSquared < (radius * radius);
    }
    else {
        // this is a box, br is a sphere
        // call algorithm for br (defined in proceding else if block)
        return br.intersectsWith(*this);
    }
}

bool BoundingRegion::operator==(BoundingRegion br){
    if (type != br.type){
        return false;
    }

    if (type == BoundTypes::AABB){
        return min == br.min && max == br.max;
    }
    else {
        return center == br.center && radius == br.radius;
    }
}