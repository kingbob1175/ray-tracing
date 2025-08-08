#pragma once

#include "ray/ray.hpp"
#include "geometry/geometry.hpp"
#include "basicmath/math.hpp"
#include "shape/intersection.hpp"
#include "light/light.hpp"

class Triangle;

class AreaLight: public Light {
public:
    float area;
    std::shared_ptr<Triangle> triangle;

    AreaLight(std::shared_ptr<Triangle> triangle, const Vector3d& radiance);

    AreaLight(const Vector3d& rad, std::string _name);

    Lightsampleresult sample(const Intersection& intersection) const override;
    
    float getpower() const override;

    float getArea() const override;
};