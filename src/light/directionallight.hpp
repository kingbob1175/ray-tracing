#pragma once

#include "ray/ray.hpp"
#include "geometry/geometry.hpp"
#include "basicmath/math.hpp"
#include "shape/intersection.hpp"
#include "light/light.hpp"

class DirectionalLight : public Light {
public:
    Vector3d direction;   
    Vector3d radiance; 

    DirectionalLight(const Vector3d& dir, const Vector3d& r)
        : direction(dir.normalize()), radiance(r) {
        type = LightType::Directionallight;
    }

    Lightsampleresult sample(const Intersection& inter) const override {
        return {
            radiance,
            Vector3d() - direction,
            MY_INFINITY,        
            1.0f,           
            type
        };
    }

    float getpower() const override {
        return radiance.norm(); 
    }
};