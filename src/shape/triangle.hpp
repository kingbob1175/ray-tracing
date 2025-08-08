#pragma once

#include "shape/shape.hpp"
#include "geometry/geometry.hpp"
#include "ray/ray.hpp"
#include "shape/intersection.hpp"
#include "loader.hpp"
#include <vector>

class Light;
class BVH;
class Material;

class Triangle: public Shape {
public:
    Point3d v1, v2, v3;
    Vector3d normal;
    Vector3d e1, e2; // edge vectors
    Vector3d n1, n2, n3; // normal vectors
    Vector2d t0, t1, t2; // texture coordinates
    float area;
    Material* mat;
    Light* light;
    Bound3 bound;

    // Triangle() {}

    Triangle(const Point3d& _v1, const Point3d& _v2, const Point3d& _v3, 
        const Vector3d& _n1, const Vector3d& _n2, const Vector3d& _n3, const Vector2d& _t0, 
        const Vector2d& _t1, const Vector2d& _t2, Material* _mat = nullptr, Light* _light = nullptr);

    Triangle(const Point3d& _v1, const Point3d& _v2, const Point3d& _v3, Material* _mat = nullptr, Light* _light = nullptr);

    // Moller-Trumbore intersection algorithm
    bool intersect(const Ray& ray) const override;

    Intersection get_intersection(const Ray& ray) const override;

    Vector2d getuv(const Vector3d& Barycentric) const;

    // compute the barycentric coord (u, v, w)
    Vector3d getBarycentric(const Point3d& p) const;

    float getArea() const override {
        return area;
    }

    Bound3 getbound() const override;

    Material* getMaterial() const override {
        return this->mat;
    }

    Light* getLight() const override {
        return this->light;
    }

    bool hasEmit() const override { return getLight() != nullptr; }
};

class TriangleMesh: public Shape {
public:

    TriangleMesh(Mesh& mesh);

    const std::vector<std::shared_ptr<Triangle>>& getTriangles() const {
        return triangles;
    }

    Bound3 getbound() const override {
        return boundingbox;
    }
    
    Vector3d evalDiffuseColor(const Vector2d& st) const;

    float getArea() const override{
        return area;
    }

    bool intersect(const Ray& ray) const override {
        return true;
    }

    Intersection get_intersection(const Ray& ray) const override;

    std::vector<Point3d> vertices;
    std::vector<Vector3d> normals;     
    std::vector<Vector2d> uvs;         
    std::vector<Face_indices> indices;

    std::vector<std::shared_ptr<Triangle>> triangles;  
    Material* mat;
    Light* light;

    uint32_t num_triangle;
    Bound3 boundingbox;
    float area;

    BVH* bvh;
};