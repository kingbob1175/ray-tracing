#pragma once

#include "geometry/geometry.hpp"
#include "basicmath/math.hpp"

class Shape;
class Material;

struct Intersection {
    Intersection() {
        intersected = false;
        coord = Point3d();
        uv = Vector2d();
        normal = Vector3d();
        distance = MY_INFINITY; 
        obj = nullptr;
        mat = nullptr;
    }
    bool intersected;
    Point3d coord;
    Vector2d uv;
    Vector3d normal;
    float distance;
    const Shape* obj;
    Material* mat;
};