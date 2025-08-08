#include "ray/ray.hpp"
#include "geometry/geometry.hpp"
#include "basicmath/math.hpp"
#include "shape/intersection.hpp"
#include "light/light.hpp"
#include "shape/triangle.hpp"
#include "shape/shape.hpp"
#include "arealight.hpp"

AreaLight::AreaLight(std::shared_ptr<Triangle> triangle, const Vector3d& rad):
        triangle(triangle)
{
    area = triangle->area;
    type = LightType::Arealight;
    radiance = rad;
}

AreaLight::AreaLight(const Vector3d& rad, std::string _name)
{
    type = LightType::Arealight;
    name = _name;
    radiance = rad;
}

Lightsampleresult AreaLight::sample(const Intersection& intersection) const {
    Lightsampleresult result;
    result.type = LightType::Arealight;

    // float r1 = get_random_number(), r2 = get_random_number();
    
    Vector3d a1 = triangle->v1 - Point3d();
    Vector3d a2 = triangle->v2 - Point3d();
    Vector3d a3 = triangle->v3 - Point3d();
    // Vector3d sample_vec = a1*(1.0f-r1-r2) + r1*a2 + r2*a3;

    float x = std::sqrt(get_random_number()), y = get_random_number();
    Vector3d sample_vec = a1 * (1.0f - x) + a2 * (x * (1.0f - y)) + a3 * (x * y);

    Point3d sample_pos = Point3d() + sample_vec;
    
    result.dir = (sample_pos - intersection.coord).normalize();
    result.distance = (sample_pos - intersection.coord).norm();
    result.pdf = 1.0f / area;
    result.radiance = radiance;

    return result;
}

float AreaLight::getpower() const {
    return radiance.norm() * area;
}

float AreaLight::getArea() const {
    return area;
}