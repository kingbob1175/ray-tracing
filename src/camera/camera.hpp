#pragma once

#include "ray/ray.hpp"
#include "basicmath/transformation.hpp"
#include "basicmath/math.hpp"
#include "geometry/geometry.hpp"

class Camera {
public:
    Camera() = default;

    Camera(int width, int height, float fov, const Point3d& eye, const Point3d& target, const Vector3d& up):
        width(width), height(height), fov(fov), eye(eye), target(target), up(up) {}
    
    Ray generateRay(int xpixel, int ypixel) const {
        Transformation cameratoworld = LookAt(eye, target, up);
        float aspectRatio = float(width) / float(height);
        float scale = std::tan(degree_to_radian(fov * 0.5f));
        float x = (2.0f * (xpixel + 0.5f) / (float)width - 1.0f) * scale * aspectRatio;
        float y = (1 - 2.0f * (ypixel + 0.5f) / (float)height) * scale;
        Vector3d dir = Vector3d(-x, y, -1.0f).normalize();
        Point3d origin = Point3d(0.0f, 0.0f, 0.0f);
        Ray r = Ray(origin, dir);
        return cameratoworld(r);
    }    
    
    int width, height;
    float fov;
    Point3d eye, target;
    Vector3d up;
};