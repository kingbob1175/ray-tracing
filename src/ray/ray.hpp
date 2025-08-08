#pragma once
#include "geometry/vector.hpp"
#include "geometry/point.hpp"
#include <iostream>
#include "basicmath/math.hpp"

class Ray{
public:
    Point3d origin;
    Vector3d dir; // normalized direction
    Vector3d dir_inv;
    float time;

    Ray(const Point3d& origin, const Vector3d& dir, const float time = 0.0) : origin(origin), dir(dir), time(time) {
        dir_inv = Vector3d();
        dir_inv.x = (dir.x == 0.0f) ? MY_INFINITY : 1 / dir.x;
        dir_inv.y = (dir.y == 0.0f) ? MY_INFINITY : 1 / dir.y;
        dir_inv.z = (dir.z == 0.0f) ? MY_INFINITY : 1 / dir.z;
    }

    Point3d at(float t) const {
        return origin + dir * t;
    }

    Point3d getorigin() const {
        return origin;
    }

    Vector3d getdir() const {
        return dir;
    }

    float gettime() const {
        return time;
    }

    friend std::ostream &operator<<(std::ostream &os, const Ray &r){
        os << "direction:" << r.dir << ", origin:" << r.origin << ", time:" << r.time << "]\n";
        return os;
    }
};