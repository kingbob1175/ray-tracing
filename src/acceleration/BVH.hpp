#pragma once

#include "basicmath/math.hpp"
#include "geometry/geometry.hpp"
#include "ray/ray.hpp"
#include "shape/shape.hpp"
#include "shape/intersection.hpp"
#include <vector>
#include <chrono>


enum BVHBuildMethod { SAH, MIDDLE };

struct BVHNode {
    BVHNode *left = nullptr, *right = nullptr;
    Bound3 box;
    float area;
    std::vector<std::shared_ptr<Shape>> shape;
    BVHNode(): box(), area(0.0f) {}
};

struct BucketInfo {
    Bound3 box;                        
    std::vector<std::shared_ptr<Shape>> shapes; 
    int count;
    BucketInfo() : box(), count(0) {}
};

class BVH {
public:

    BVH(const std::vector<std::shared_ptr<Shape>>& _shapes, int maxPrimsInNode, BVHBuildMethod buildMethod);

    BVHNode* root;

    // intersect the ray with the BVH from the root node and return the intersection information
    Intersection Intersect(const Ray& ray) const;

    // get the intersection information for the ray with the BVH from the given node
    Intersection getIntersection(const Ray& ray, const BVHNode* node, int depth=0) const; 

    // check if the ray intersects the BVH from the root
    // bool intersectP(const Ray& ray) const;

    BVHNode* buildBVH(std::vector<std::shared_ptr<Shape>>& s, BVHBuildMethod buildMethod) ;
    
    // build the BVH using the SAH algorithm
    BVHNode* SAHBuildNode(std::vector<std::shared_ptr<Shape>>& shapes);

    // build the BVH using the middle split algorithm
    BVHNode* MIDDLEBuildNode(std::vector<std::shared_ptr<Shape>>& shapes);


    BVHNode* buildWithBucketSAH(std::vector<std::shared_ptr<Shape>>& shapes,
                                const Bound3& parentBox, int dim, int totalShapes,
                                int depth);

    BVHNode* fallbackSpatialSplit(std::vector<std::shared_ptr<Shape>>& shapes,
                                  const Bound3& parentBox, int dim, int depth);

    BVHNode* createLeaf(std::vector<std::shared_ptr<Shape>>& shapes, 
                        const Bound3& box);

    const int maxPrimsInNode; // maximum number of primitives in a node
    const BVHBuildMethod buildMethod; // method used to build the BVH
    std::vector<std::shared_ptr<Shape>> shapes; // list of shapes to build the BVH for
    mutable std::chrono::duration<double> time_intersection{0};

    mutable long long int visitedLeafCount = 0;
    mutable long long int visitedLeafDepthSum = 0;
    mutable int maxVisitedLeafDepth = 0;
    mutable int minVisitedLeafDepth = 10000;    
};