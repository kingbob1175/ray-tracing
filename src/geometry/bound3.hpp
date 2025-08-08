#pragma once

#include "vector.hpp"
#include "point.hpp"
#include "basicmath/math.hpp"
#include "ray/ray.hpp"
#include <algorithm>
#include <array>

class Bound3 {
public:
    Point3d pMin, pMax;

    Bound3() {
        pMax = Point3d(-MY_INFINITY, -MY_INFINITY, -MY_INFINITY);
        pMin = Point3d(MY_INFINITY, MY_INFINITY, MY_INFINITY);
    }

    Bound3(const Point3d& point1, const Point3d& point2): pMin(Point3d(std::min(point1.x, point2.x), 
        std::min(point1.y, point2.y), std::min(point1.z, point2.z))), pMax(Point3d(std::max(point1.x, point2.x), 
        std::max(point1.y, point2.y), std::max(point1.z, point2.z))) {}

    Bound3(const Point3d& point): pMin(point), pMax(point) {}

    const Point3d &operator[] (int i) const {}
    Point3d &operator[] (int i) {}

    // return the corner of the box
    Point3d Corner(int i){
        return Point3d(
        (*this)[(i & 1)].x, // (i & 1) is 1 if i is odd(for xmax), 0 if i is even(xmin)
        (*this)[(i & 2) ? 1 : 0].y,
        (*this)[(i & 4) ? 1 : 0].z
        );
    }
   
    // return the central point of the box
    Point3d Central() const {
        return Point3d((pMin.x + pMax.x) / 2.0f, (pMin.y + pMax.y) / 2.0f, (pMin.z + pMax.z) / 2.0f);
    }

    // return the diagonal of the bounding box
    Vector3d Diagonal() const {
        return pMax - pMin;
    }

    // return the surface area of the bounding box
    float Surface() const {
        Vector3d p = this->Diagonal();
        return 2.0f * (p.x * p.y + p.x * p.z + p.y * p.z);
    }

    // return the volume of the bounding box
    float Volume() const {
        Vector3d p = this->Diagonal();
        return p.x * p.y * p.z;
    }

    // return which axis is the longest, 0 for x, 1 for y, 2 for z
    int Maxextent() const {
        Vector3d p = this->Diagonal();
        if(p.x > p.y && p.x > p.z) {
            return 0;
        }else if(p.y > p.z){
            return 1;
        }else{
            return 2;
        }
    }

    Point3d Lerp(const Point3d& t) const {
        return Point3d(lerp(t.x, pMin.x, pMax.x),
                        lerp(t.y, pMin.y, pMax.y),
                        lerp(t.z, pMin.z, pMax.z));
    }

    Vector3d Offset (const Point3d& p) {
        Vector3d d = this->Diagonal();
        return Vector3d((p.x-pMin.x)/d.x, (p.y-pMin.y)/d.y, (p.z-pMin.z)/d.z);
    }
};

// return a new bounding box that contains both this and a point
inline Bound3 Union(const Point3d& p, const Bound3& b) {
    return Bound3(Point3d(std::min(p.x, b.pMin.x), std::min(p.y, b.pMin.y), std::min(p.z, b.pMin.z)),
    Point3d(std::max(p.x, b.pMax.x), std::max(p.y, b.pMax.y), std::max(p.z, b.pMax.z)));
}

// return a new bounding box that contains both this and another bounding box
inline Bound3 Union(const Bound3& b1, const Bound3& b2) {
    return Bound3(Point3d(std::min(b1.pMin.x, b2.pMin.x), std::min(b1.pMin.y, b2.pMin.y), std::min(b1.pMin.z, b2.pMin.z)),
    Point3d(std::max(b1.pMax.x, b2.pMax.x), std::max(b1.pMax.y, b2.pMax.y), std::max(b1.pMax.z, b2.pMax.z)));
}

// return true if two bounding boxes overlap
inline bool Overlaps(const Bound3& b1, const Bound3& b2) {
    bool x = (b2.pMax.x >= b1.pMin.x) && (b2.pMin.x <= b1.pMax.x);
    bool y = (b2.pMax.y >= b1.pMin.y) && (b2.pMin.y <= b1.pMax.y);
    bool z = (b2.pMax.z >= b1.pMin.z) && (b2.pMin.z <= b1.pMax.z);
    return (x && y && z);
}

// return the intersection of two bounding boxes
inline Bound3 Intersect(const Bound3& b1, const Bound3& b2) {
    return Bound3(Point3d(std::max(b2.pMin.x, b1.pMin.x), std::max(b2.pMin.y, b1.pMin.y), std::max(b2.pMin.z, b1.pMin.z)),
    Point3d(std::min(b2.pMax.x, b1.pMax.x), std::min(b2.pMax.y, b1.pMax.y), std::min(b2.pMax.z, b1.pMax.z)));
}

// return true if the point is inside the bounding box
inline bool Inside(const Point3d& p, const Bound3& b) {
    return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y && p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
}

// return true if the ray intersects the bounding box
inline bool IntersectP(const Ray& ray, const Bound3& b, std::array<int, 3> dirIsNeg, float tMin) {

    float tx_min = (b.pMin.x - ray.origin.x) * ray.dir_inv.x;
    float tx_max = (b.pMax.x - ray.origin.x) * ray.dir_inv.x;
    if (dirIsNeg[0]) std::swap(tx_min, tx_max);

    float ty_min = (b.pMin.y - ray.origin.y) * ray.dir_inv.y; 
    float ty_max = (b.pMax.y - ray.origin.y) * ray.dir_inv.y;
    if (dirIsNeg[1]) std::swap(ty_min, ty_max);

    float tz_min = (b.pMin.z - ray.origin.z) * ray.dir_inv.z;
    float tz_max = (b.pMax.z - ray.origin.z) * ray.dir_inv.z;
    if (dirIsNeg[2]) std::swap(tz_min, tz_max);

    float t_min = std::max({tx_min, ty_min, tz_min});
    float t_max = std::min({tx_max, ty_max, tz_max});

    tMin = t_min;

    return (t_max >= std::max(Epsilon, t_min));

}