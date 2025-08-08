#include "basicmath/math.hpp"
#include "geometry/geometry.hpp"
#include "ray/ray.hpp"
#include "shape/shape.hpp"
#include "shape/intersection.hpp"
#include <vector>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <array>
#include "BVH.hpp"

// Constructor
BVH::BVH(const std::vector<std::shared_ptr<Shape>>& _shapes, int maxPrimsInNode, BVHBuildMethod buildMethod): 
    shapes(_shapes), maxPrimsInNode(maxPrimsInNode), buildMethod(buildMethod)
{ 
    std::cout << "Building BVH..." << std::endl;

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    
    if (shapes.size() == 0) return;
    root = buildBVH(shapes, buildMethod);

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "BVH build time: " << elapsed_seconds.count() << " seconds" << std::endl;
}

// Intersect the ray with the BVH from the root node and return the intersection information
Intersection BVH::Intersect(const Ray& ray) const {
    Intersection isect;
    if (!root) return isect;
    // auto start = std::chrono::high_resolution_clock::now();
    isect = getIntersection(ray, root);
    // auto end = std::chrono::high_resolution_clock::now();
    // time_intersection += std::chrono::duration<double>(end - start);
    return isect;
}

// Get the intersection information for the ray with the BVH from the given node
Intersection BVH::getIntersection(const Ray& ray, const BVHNode* node, int depth) const {
    Intersection isect;
    if (!node) return isect;

    float leftMin = MY_INFINITY, rightMin = MY_INFINITY, tmin = MY_INFINITY;

    std::array<int, 3> dirIsNeg = { ray.dir.x < 0, ray.dir.y < 0, ray.dir.z < 0 };
    // auto start = std::chrono::high_resolution_clock::now();
    bool hit = IntersectP(ray, node->box, dirIsNeg, tmin);
    // auto end = std::chrono::high_resolution_clock::now();
    // time_intersection += std::chrono::duration<double>(end - start);
    if (!hit) return isect;
    
    if (!node->left && !node->right) {
        // visitedLeafCount++;
        // visitedLeafDepthSum += depth;
        // if (depth > maxVisitedLeafDepth) maxVisitedLeafDepth = depth;
        // if (depth < minVisitedLeafDepth) minVisitedLeafDepth = depth;
        for (auto& shape : node->shape) {          
            Intersection i = shape->get_intersection(ray);
            if (i.intersected && i.distance < isect.distance) {
                isect = i;
            }
        }
        return isect;
    }

    Intersection isectleft, isectright;
    isectleft = getIntersection(ray, node->left, depth+1);
    isectright = getIntersection(ray, node->right, depth+1);
    
    if (isectleft.intersected || isectright.intersected) {
        isect = isectleft.distance < isectright.distance ? isectleft : isectright; 
    }

    return isect;
}

// Build BVH
BVHNode* BVH::buildBVH(std::vector<std::shared_ptr<Shape>>& s, BVHBuildMethod buildMethod) {
    if (buildMethod == SAH) return SAHBuildNode(s);
    else if (buildMethod == MIDDLE) return MIDDLEBuildNode(s);
    else {
        std::cerr << "Invalid build method!" << std::endl;
        return nullptr;
    }
}

BVHNode* BVH::SAHBuildNode(std::vector<std::shared_ptr<Shape>>& shapes) {
    constexpr int BUCKETS = 12;
    BVHNode* node = new BVHNode();
    
    // Compute bounds of all shapes
    for (auto& shape : shapes) {
        node->box = Union(shape->getbound(), node->box);
    }
    const int n = shapes.size();
    node->area = node->box.Surface();
    
    if (n <= maxPrimsInNode || node->box.Surface() < 1e-6f) {
        node->shape = std::move(shapes);
        return node;
    }
    
    const int dim = node->box.Maxextent();
    
    return buildWithBucketSAH(shapes, node->box, dim, n, 0);
}

// Recursively build the BVH with SAH
BVHNode* BVH::buildWithBucketSAH(std::vector<std::shared_ptr<Shape>>& shapes,
                                const Bound3& parentBox, int dim, int totalShapes,
                                int depth) {
    constexpr int MAX_DEPTH = 64;
    constexpr int BUCKETS = 12;
    
    // Depth protection: prevent stack overflow
    if (depth > MAX_DEPTH) {
        return createLeaf(shapes, parentBox);
    }
    
    BVHNode* node = new BVHNode();
    node->box = parentBox;
    const int n = shapes.size();
    
    // 1. Divide shapes into buckets
    std::vector<BucketInfo> buckets(BUCKETS);
    for (auto& shape : shapes) {
        Point3d centroid = shape->getbound().Central();
        float relPos = (centroid[dim] - parentBox.pMin[dim]) / 
                      (parentBox.pMax[dim] - parentBox.pMin[dim] + 1e-6f);
        int b = std::clamp(static_cast<int>(BUCKETS * relPos), 0, BUCKETS-1);
        
        buckets[b].count++;
        buckets[b].box = Union(buckets[b].box, shape->getbound());
        buckets[b].shapes.push_back(shape);
    }
    
    // 2. Compute minimum cost split
    float minCost = FLT_MAX;
    int bestSplit = -1;
    
    // Leftbox
    std::vector<Bound3> leftBounds(BUCKETS);
    std::vector<int> leftCounts(BUCKETS);
    Bound3 leftBox;
    int leftTotal = 0;
    
    for (int i = 0; i < BUCKETS; ++i) {
        leftBox = Union(leftBox, buckets[i].box);
        leftTotal += buckets[i].count;
        leftBounds[i] = leftBox;
        leftCounts[i] = leftTotal;
    }
    
    // Rightbox
    Bound3 rightBox;
    int rightTotal = 0;
    
    for (int i = BUCKETS-1; i > 0; --i) {
        rightBox = Union(rightBox, buckets[i].box);
        rightTotal += buckets[i].count;
        
        if (leftCounts[i-1] == 0 || rightTotal == 0) continue;
        
        // Compute cost
        float cost = 1.0f + 
            (leftBounds[i-1].Surface() * leftCounts[i-1] +
             rightBox.Surface() * rightTotal) / parentBox.Surface();
        
        if (cost < minCost) {
            minCost = cost;
            bestSplit = i;
        }
    }

    // 3. Decide on split
    const float leafCost = n;
    
    // Invalid split or high cost
    if (bestSplit == -1 || minCost >= leafCost) {
        if (n > 2 * maxPrimsInNode) {
            return fallbackSpatialSplit(shapes, parentBox, dim, depth);
        }
        return createLeaf(shapes, parentBox);
    }
    
    // Create split
    std::vector<std::shared_ptr<Shape>> leftShapes, rightShapes;
    leftShapes.reserve(leftCounts[bestSplit-1]);
    rightShapes.reserve(n - leftCounts[bestSplit-1]);
    
    for (int i = 0; i < bestSplit; ++i) {
        leftShapes.insert(leftShapes.end(),
                         buckets[i].shapes.begin(),
                         buckets[i].shapes.end());
    }
    for (int i = bestSplit; i < BUCKETS; ++i) {
        rightShapes.insert(rightShapes.end(),
                          buckets[i].shapes.begin(),
                          buckets[i].shapes.end());
    }
    
    // 4. Recursively build left and right subtrees
    node->left = buildWithBucketSAH(leftShapes, leftBounds[bestSplit-1], 
                                   dim, totalShapes, depth+1);
    node->right = buildWithBucketSAH(rightShapes, rightBox,
                                    dim, totalShapes, depth+1);
    return node;
}

// Strategy when no valid split is found ----- Middle Split Strategy
BVHNode* BVH::fallbackSpatialSplit(std::vector<std::shared_ptr<Shape>>& shapes,
                                  const Bound3& parentBox, int dim, int depth) {
    auto mid = shapes.begin() + shapes.size() / 2;
    
    switch(dim) {
        case 0:
            std::nth_element(shapes.begin(), mid, shapes.end(),
                [](const auto& a, const auto& b) {
                    return a->getbound().Central().x < b->getbound().Central().x;
                });
            break;
        case 1:
            std::nth_element(shapes.begin(), mid, shapes.end(),
                [](const auto& a, const auto& b) {
                    return a->getbound().Central().y < b->getbound().Central().y;
                });
            break;
        case 2:
        std::nth_element(shapes.begin(), mid, shapes.end(),
            [](const auto& a, const auto& b) {
                return a->getbound().Central().z < b->getbound().Central().z;
            });
        break;
    }
    
    Bound3 leftBox, rightBox;
    for (auto it = shapes.begin(); it != mid; ++it) 
        leftBox = Union(leftBox, (*it)->getbound());
    for (auto it = mid; it != shapes.end(); ++it) 
        rightBox = Union(rightBox, (*it)->getbound());
    
    auto leftShapes = std::vector<std::shared_ptr<Shape>>(
        std::make_move_iterator(shapes.begin()),
        std::make_move_iterator(mid));
    
    auto rightShapes = std::vector<std::shared_ptr<Shape>>(
        std::make_move_iterator(mid),
        std::make_move_iterator(shapes.end()));
    
    BVHNode* node = new BVHNode();
    node->box = parentBox;
    node->left = buildWithBucketSAH(leftShapes, leftBox, dim, shapes.size(), depth+1);
    node->right = buildWithBucketSAH(rightShapes, rightBox, dim, shapes.size(), depth+1);
    return node;
}

BVHNode* BVH::createLeaf(std::vector<std::shared_ptr<Shape>>& shapes, 
                        const Bound3& box) {
    BVHNode* node = new BVHNode();
    node->box = box;
    node->shape = std::move(shapes);
    return node;
}

// // build the BVH using the SAH algorithm
// BVHNode* BVH::SAHBuildNode(std::vector<std::shared_ptr<Shape>>& shapes) {
//     BVHNode* node = new BVHNode();
    
//     int n = shapes.size(); 

//     for (auto& shape : shapes) {
//         node->box = Union(shape->getbound(), node->box);
//     }
//     node->area = node->box.Surface();

//     if (n <= maxPrimsInNode) {
//         node->shape = shapes;
//         node->left = nullptr;
//         node->right = nullptr;
//         return node;
//     }
    
//     float mincost = MY_INFINITY;
//     int mincostIndex = -1;

//     switch(node->box.Maxextent()) {
//         case 0:
//             std::sort(shapes.begin(), shapes.end(), [](const std::shared_ptr<Shape>& s1, const std::shared_ptr<Shape>& s2) {
//                 return s1->getbound().Central().x < s2->getbound().Central().x;
//             });
//             break;
//         case 1:
//             std::sort(shapes.begin(), shapes.end(), [](const std::shared_ptr<Shape>& s1, const std::shared_ptr<Shape>& s2) {
//                 return s1->getbound().Central().y < s2->getbound().Central().y;
//             });
//             break;
//         case 2:
//             std::sort(shapes.begin(), shapes.end(), [](const std::shared_ptr<Shape>& s1, const std::shared_ptr<Shape>& s2) {
//                 return s1->getbound().Central().z < s2->getbound().Central().z;
//             });
//             break;
//         default:
//             std::cerr << "Error: Maxextent() returned invalid value" << std::endl;
//             return nullptr;
//     }

//     std::vector<Bound3> leftboxes(n);
//     leftboxes[0] = shapes[0]->getbound();
//     for (int j = 1; j < n; j++) {
//         leftboxes[j] = Union(shapes[j]->getbound(), leftboxes[j-1]);
//     }

//     std::vector<Bound3> rightboxes(n);
//     rightboxes[n-1] = shapes[n-1]->getbound();
//     for (int j = n-2; j >= 0; j--) {
//         rightboxes[j] = Union(shapes[j]->getbound(), rightboxes[j+1]);
//     }

//     for (int i = 1; i < n; i++) {
//         float p_left = leftboxes[i-1].Surface() / node->box.Surface();
//         float p_right = rightboxes[i].Surface() / node->box.Surface();
//         float cost = 0.125f + p_left * i + p_right * (n - i);
//         if (cost < mincost) {
//             mincost = cost;
//             mincostIndex = i;
//         }
//     }

//     auto leftshapes = std::vector<std::shared_ptr<Shape>>(
//         std::make_move_iterator(shapes.begin()),
//         std::make_move_iterator(shapes.begin() + mincostIndex));

//     auto rightshapes = std::vector<std::shared_ptr<Shape>>(
//         std::make_move_iterator(shapes.begin() + mincostIndex),
//         std::make_move_iterator(shapes.end()));
        

//     assert(leftshapes.size() + rightshapes.size() == n);

//     node->left = SAHBuildNode(leftshapes);
//     node->right = SAHBuildNode(rightshapes);
    
//     return node;
// }


// Build the BVH using the middle split algorithm
BVHNode* BVH::MIDDLEBuildNode(std::vector<std::shared_ptr<Shape>>& shapes) {
    BVHNode* node = new BVHNode();

    int n = shapes.size(); 

    for (auto& shape : shapes) {
        node->box = Union(shape->getbound(), node->box);
    }
    node->area = node->box.Surface();

    if (n <= maxPrimsInNode) {
        node->shape = shapes;
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }

    switch(node->box.Maxextent()) {
        case 0:
            std::sort(shapes.begin(), shapes.end(), [](const std::shared_ptr<Shape>& s1, const std::shared_ptr<Shape>& s2) {
                return s1->getbound().Central().x < s2->getbound().Central().x;
            });
            break;
        case 1:
            std::sort(shapes.begin(), shapes.end(), [](const std::shared_ptr<Shape>& s1, const std::shared_ptr<Shape>& s2) {
                return s1->getbound().Central().y < s2->getbound().Central().y;
            });
            break;
        case 2:
            std::sort(shapes.begin(), shapes.end(), [](const std::shared_ptr<Shape>& s1, const std::shared_ptr<Shape>& s2) {
                return s1->getbound().Central().z < s2->getbound().Central().z;
            });
            break;
        default:
            std::cerr << "Error: Maxextent() returned invalid value" << std::endl;
            return nullptr;
    }

    int i = shapes.size() / 2;
    // auto leftshapes = std::vector<std::shared_ptr<Shape>>(shapes.begin(), shapes.begin() + i);
    // auto rightshapes = std::vector<std::shared_ptr<Shape>>(shapes.begin() + i, shapes.end());
    
    auto leftshapes = std::vector<std::shared_ptr<Shape>>(
        std::make_move_iterator(shapes.begin()),
        std::make_move_iterator(shapes.begin() + i));

    auto rightshapes = std::vector<std::shared_ptr<Shape>>(
        std::make_move_iterator(shapes.begin() + i),
        std::make_move_iterator(shapes.end()));
        
    assert(leftshapes.size() + rightshapes.size() == shapes.size());

    node->left = MIDDLEBuildNode(leftshapes);
    node->right = MIDDLEBuildNode(rightshapes);
    
    return node;
}
