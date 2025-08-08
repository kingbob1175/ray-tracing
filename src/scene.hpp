#pragma once

#include "geometry/geometry.hpp"
#include "basicmath/math.hpp"
#include "shape/shape.hpp"
#include "shape/intersection.hpp"
#include "light/light.hpp"
#include "material/mat.hpp"
#include "acceleration/BVH.hpp"

#include <vector>
#include <string>
#include <chrono>

class Scene {
public:
    Scene() = default;

    std::vector<std::shared_ptr<Shape>> shapes;
    std::vector<std::shared_ptr<Light>> lights;

    float RussianRoulette = 0.8f;
    int maxDepth = 5;

    std::vector<float> light_weights;
    std::vector<float> light_cdf;
    float total_light_weight = 0.f;
    void InitLightCDF();

    Lightsampleresult SampleLights(Intersection inter) const;

    Vector3d Shade(const Ray& ray, int depth) const ;

    void add_shape(std::shared_ptr<Shape> shape) {
        shapes.push_back(shape);
    }

    void add_shape(std::vector<std::shared_ptr<Shape>> shape) {
        shapes.insert(shapes.end(), shape.begin(), shape.end());
    }

    void add_light(std::shared_ptr<Light> light) {
        lights.push_back(light);
    }

    void add_light(std::vector<std::shared_ptr<Light>> light) {
        lights.insert(lights.end(), light.begin(), light.end());
    }

    void buildbvh(int maxPrimsInNode, BVHBuildMethod method) {
        bvh = new BVH(shapes, maxPrimsInNode, method);
    }

    Intersection intersect(const Ray& ray) const {
        auto start = std::chrono::high_resolution_clock::now();

        Intersection isect = this->bvh->Intersect(ray);

        auto end = std::chrono::high_resolution_clock::now();
        time_intersection += std::chrono::duration<double>(end - start);
        return isect;
        // return this->bvh->Intersect(ray);
    }
    
    Intersection intersect_noacceleration(const Ray& ray) const {
        auto start = std::chrono::high_resolution_clock::now();

        Intersection isect;
        for (auto shape : shapes) {
            Intersection shape_isect = shape->get_intersection(ray);
            if (shape_isect.intersected && shape_isect.distance < isect.distance) {
                isect = shape_isect;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        time_intersection += std::chrono::duration<double>(end - start);

        return isect;
    }

    BVH* bvh;
    mutable std::chrono::duration<double> time_intersection{0};
    mutable std::chrono::duration<double> time_lightsampling{0};
};
