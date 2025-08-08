#pragma once

#include "ray/ray.hpp"
#include "shape/intersection.hpp"
#include "basicmath/math.hpp"
#include "geometry/geometry.hpp"

class Material;
class Light;

class Shape{
public:
    Shape() {}
    virtual ~Shape() {}
    virtual bool intersect(const Ray& ray) const = 0;
    virtual Intersection get_intersection(const Ray& ray) const = 0;
    virtual float getArea() const = 0;
    virtual Bound3 getbound() const = 0;
    virtual const Material* getMaterial() const { return nullptr; }
    virtual const Light* getLight() const { return nullptr; }
    virtual bool hasEmit() const { return getLight() != nullptr; }
};