#include "shape/shape.hpp"
#include "geometry/geometry.hpp"
#include "ray/ray.hpp"
#include "shape/intersection.hpp"
#include <vector>
#include <cmath>
#include <array>
#include "loader.hpp"
#include "material/mat.hpp"
#include "acceleration/BVH.hpp"
#include "light/arealight.hpp"
#include "triangle.hpp"
#include <unordered_map>

Triangle::Triangle(const Point3d& _v1, const Point3d& _v2, const Point3d& _v3, 
    const Vector3d& _n1, const Vector3d& _n2, const Vector3d& _n3, const Vector2d& _t0, 
    const Vector2d& _t1, const Vector2d& _t2, Material* _mat, Light* _light): 
    v1(_v1), v2(_v2), v3(_v3), n1(_n1), n2(_n2), n3(_n3), t0(_t0), t1(_t1), t2(_t2), mat(_mat), light(_light) 
{
    e1 = v2 - v1;
    e2 = v3 - v1;
    normal = e1.cross(e2).normalize();
    area = 0.5f * e1.cross(e2).norm();
    bound = Union(v3, Bound3(v1, v2));

    if (mat == nullptr && light == nullptr) {
        std::cerr << "Triangle: both mat and light are nullptr" << std::endl;
        exit(1);
    }
} 

Triangle::Triangle(const Point3d& _v1, const Point3d& _v2, const Point3d& _v3, Material* _mat, Light* _light): 
    v1(_v1), v2(_v2), v3(_v3), mat(_mat), light(_light) 
{
    e1 = v2 - v1;
    e2 = v3 - v1;
    normal = e1.cross(e2).normalize();
    area = 0.5f * e1.cross(e2).norm();
    bound = Union(v3, Bound3(v1, v2));
}

bool Triangle::intersect(const Ray& ray) const {
    Vector3d ray_cross_e2 = ray.dir.cross(e2);
    float det = e1.dot(ray_cross_e2);

    if (det > -Epsilon && det < Epsilon) { // ray is parallel to the plane
        return false;
    }

    float inv_det = 1.0f / det;
    Vector3d s = ray.origin - v1;
    float u = inv_det * s.dot(ray_cross_e2);

    if( u < 0 || u > 1 ) {
        return false;
    }

    Vector3d s_cross_e1 = s.cross(e1);
    float v = inv_det * ray.dir.dot(s_cross_e1);

    if(v < 0 || u + v > 1 ) {
        return false;
    }

    float t = inv_det * e2.dot(s_cross_e1);

    if (t < Epsilon) {
        return false;
    } else {
        return true;
    }
}

Intersection Triangle::get_intersection(const Ray& ray) const {
    Intersection inter;
    
    Vector3d ray_cross_e2 = ray.dir.cross(e2);
    float det = e1.dot(ray_cross_e2);
 
    if (fabs(det) < Epsilon) { // ray is parallel to the plane
        return inter;
    }

    float inv_det = 1.0f / det;
    Vector3d s = ray.origin - v1;
    float u = inv_det * s.dot(ray_cross_e2);

    if( u < 0 || u > 1 ) {
        return inter;
    }

    Vector3d s_cross_e1 = s.cross(e1);
    float v = inv_det * ray.dir.dot(s_cross_e1);

    if( v < 0 || u + v > 1 ) {
        return inter;
    }

    float t = inv_det * e2.dot(s_cross_e1);

    if (t < 0) {
        return inter;
    } else {
        inter.intersected = true;
        inter.normal = normal;
        inter.distance = t;
        inter.obj = this;
        inter.coord = ray.at(t) + normal * 1e-5f * ray.at(t).maxdir();
        inter.uv = getuv(getBarycentric(ray.at(t)));
        return inter;
    }
}

Vector2d Triangle::getuv (const Vector3d& Barycentric) const {
    Vector2d uv = Barycentric.x * t0 + Barycentric.y * t1 + Barycentric.z * t2;
    if (uv.x < -Epsilon || uv.x > 1+Epsilon || uv.y < -Epsilon || uv.y > 1+Epsilon) {
        // std::cout << "Triangle::getuv: uv out of range: (" << uv.x << "," << uv.y << ")" << std::endl;
    }
    return uv;
}

Vector3d Triangle::getBarycentric(const Point3d& p) const {
    Vector3d a0 = v1 - p, a1 = v2 - p, a2 = v3 - p;
    float S0 = 0.5f * a1.cross(a2).norm();
    float S1 = 0.5f * a2.cross(a0).norm();
    float S2 = 0.5f * a0.cross(a1).norm();
    float S_total = S1 + S2 + S0;
    float u = S0 / S_total;
    float v = S1 / S_total;
    float w = S2 / S_total;
    return Vector3d(u, v, w);
}

Bound3 Triangle::getbound() const {
    return bound; 
}

TriangleMesh::TriangleMesh(Mesh& mesh)
{
    vertices = mesh.vertices;
    normals = mesh.normals;
    uvs = mesh.texcoords;
    indices = mesh.faces;

    Point3d min_vertice(MY_INFINITY, MY_INFINITY, MY_INFINITY);
    Point3d max_vertice(-MY_INFINITY, -MY_INFINITY, -MY_INFINITY);

    area = 0;

    for (size_t i = 0; i < indices.size(); i++) {
        Material* mat = nullptr;
        Light* light = nullptr;
        const auto& matName = indices[i].material_name;

        // find material and light in cache
        if (mesh.materialCache.count(matName)) { 
            mat = mesh.materialCache[matName];
        } else if (mesh.lightCache.count(matName)) {
            light = mesh.lightCache[matName];
            
        } else {
            std::cerr << "TriangleMesh: material or light not found in cache" << std::endl;
            exit(1);
        } 

        for (auto& tri_idx : indices[i].vertex_indices) {
            auto tri = std::make_shared<Triangle>(vertices[tri_idx[0]], vertices[tri_idx[1]], vertices[tri_idx[2]], 
                normals[tri_idx[0]], normals[tri_idx[1]], normals[tri_idx[2]], uvs[tri_idx[0]], 
                uvs[tri_idx[1]], uvs[tri_idx[2]], mat, light);
            // auto tri = std::make_shared<Triangle>(vertices[tri_idx[0]], vertices[tri_idx[1]], vertices[tri_idx[2]], mat, light);
            triangles.push_back(tri);
            area += tri->area;

            max_vertice = Max(max_vertice, vertices[tri_idx[0]]);
            min_vertice = Min(min_vertice, vertices[tri_idx[0]]);
            max_vertice = Max(max_vertice, vertices[tri_idx[1]]);
            min_vertice = Min(min_vertice, vertices[tri_idx[1]]);
            max_vertice = Max(max_vertice, vertices[tri_idx[2]]);
            min_vertice = Min(min_vertice, vertices[tri_idx[2]]);
        }
    }
    boundingbox = Bound3(min_vertice, max_vertice);
    num_triangle = triangles.size();

    std::cout << "TriangleMesh: num_triangle = " << num_triangle << std::endl;

    // auto shapes = ptr_cast_vector<Triangle, Shape>(triangles);
    // bvh = new BVH(shapes, 4, BVHBuildMethod::SAH);
}


Vector3d TriangleMesh::evalDiffuseColor(const Vector2d& st) const
{
    float scale = 5.0f;
    int pattern = (fmodf(st.x * scale, 1.0f) > 0.5f) ^ (fmodf(st.y * scale, 1.0f) > 0.5f);
    return lerp((float)pattern, Vector3d(0.815f, 0.235f, 0.031f),
                Vector3d(0.937f, 0.937f, 0.231f));
}

Intersection TriangleMesh::get_intersection(const Ray& ray) const {
    return bvh->Intersect(ray);
}