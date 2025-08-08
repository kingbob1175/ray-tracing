#pragma once

#include "ray/ray.hpp"
#include "geometry/geometry.hpp"
#include "basicmath/math.hpp"
#include "shape/intersection.hpp"
#include "light/light.hpp"

class EnvironmentLight: public Light{
public:
    Vector3d radiance;

    EnvironmentLight(const Vector3d& r): radiance(r) {
        type = LightType::Environmentlight;
    }

    Lightsampleresult sample(const Intersection& intersection) const override {
        Lightsampleresult result;

        // Sample direction from hemisphere
        float r1 = 2.0f*get_random_number()-1.0f, r2 = get_random_number();
        float r = std::sqrtf(1.0f - r1*r1);
        float phi = 2.0f*PI*r2;
        Vector3d dir = Vector3d(r*std::cos(phi), r*std::sin(phi), r1);

        result.dir = dir;
        result.distance = MY_INFINITY;
        result.pdf = 1.0f / (4.0f*PI); // uniform distribution over the whole hemisphere
        result.radiance = radiance;
        result.type = LightType::Environmentlight;

        return result;
    }

    float getpower() const override {
        return radiance.norm() * 4.0f * PI;
    }
};