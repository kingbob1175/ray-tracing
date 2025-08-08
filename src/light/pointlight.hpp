#pragma once

#include "ray/ray.hpp"
#include "geometry/geometry.hpp"
#include "basicmath/math.hpp"
#include "shape/intersection.hpp"
#include "light/light.hpp"

class Pointlight: public Light {
public:
    Point3d pos;
    Vector3d intensity;

    Pointlight(const Point3d& pos, const Vector3d& intensity) : pos(pos), intensity(intensity) {
        type = LightType::Pointlight;
    }

    Lightsampleresult sample(const Intersection& intersection) const override {
        Lightsampleresult result;

        result.type = type;
        result.dir = (pos - intersection.coord).normalize();
        result.distance = (pos - intersection.coord).norm();
        result.radiance = intensity / (result.distance*result.distance);
        result.pdf = 1.0f;
        return result;
    }

    float getpower() const override {
        return intensity.norm();
    }
};