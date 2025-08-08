#pragma once

#include "shape/shape.hpp"
#include "geometry/geometry.hpp"
#include "ray/ray.hpp"
#include "shape/intersection.hpp"
#include "material/mat.hpp"
#include "basicmath/math.hpp"

class sphere: public Shape {
public:
    sphere(const Point3d& _center, float _radius, Material* _m, Light* _l): center(_center), radius(_radius), mat(_m), light(_l) {}

    Bound3 getbound() const override {
        return Bound3(center + Vector3d(-radius, -radius, -radius), center + Vector3d(radius, radius, radius));
    }

    float getArea() const override {
        return 4 * PI * radius * radius; 
    }

    bool intersect(const Ray& ray) const override {
        Vector3d oc = ray.origin - center;
        float a = ray.dir.dot(ray.dir);
        float b = 2 * ray.dir.dot(oc);
        float c = oc.dot(oc) - radius * radius;
        float t1, t2;
        if (!quadratic(a, b, c, t1, t2)) {
            return false;
        } else {
            if (t1 < 0) t1 = t2;
            if (t1 < 0) return false;
            return true; 
        }
    }

    Intersection get_intersection(const Ray& ray) const override {
        Vector3d oc = ray.origin - center;
        float a = ray.dir.dot(ray.dir);
        float b = 2 * ray.dir.dot(oc);
        float c = oc.dot(oc) - radius * radius;
        float t1, t2;
        if (!quadratic(a, b, c, t1, t2)) {
            return Intersection();
        } else {
            if (t1 < 0) t1 = t2;
            if (t1 < 0) return Intersection();
            Intersection inter;
            inter.intersected = true;   
            inter.coord = ray.origin + ray.dir * t1;
            inter.normal = (inter.coord - center).normalize();
            inter.distance = t1;
            inter.obj = this;
            inter.mat = mat;
            return inter;
        }
    }

    Material* getMaterial() const override {
        return this->mat;
    }

    Light* getLight() const override {
        return this->light;
    }

    bool hasEmit() const override { return getLight() != nullptr; }

    Point3d center;
    float radius;
    Material *mat;
    Light *light;
};