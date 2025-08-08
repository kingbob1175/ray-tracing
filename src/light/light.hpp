#pragma once

#include "ray/ray.hpp"
#include "geometry/geometry.hpp"
#include "basicmath/math.hpp"
#include "shape/intersection.hpp"
#include <string>

enum LightType { Pointlight, Arealight, Environmentlight, Directionallight };

struct Lightsampleresult {
    Vector3d radiance;
    Vector3d dir; // direction to light source
    float distance; // distance to light source
    float pdf;
    LightType type;
    // bool isdelta;
};

class Light {
public:
    Light() = default;

    //sample the light
    virtual Lightsampleresult sample(const Intersection& intersection) const = 0;

    // return intensity of light source
    virtual float getpower() const = 0;

    virtual float getArea() const = 0;     

    LightType type;
    std::string name;
    Vector3d radiance;
    int index;
};