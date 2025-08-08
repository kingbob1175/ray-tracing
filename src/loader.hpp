#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <unordered_map>
#include "geometry/geometry.hpp"
#include "camera/camera.hpp"
#include "tiny_obj_loader.h"
#include "tinyxml2.h"
#include "basicmath/math.hpp"

class TriangleMesh;
class Shape;
class Light;
class Camera;
class Material;

struct Lightinfo {
    std::string name;
    Vector3d radiance;
};

struct Face_indices {
    std::vector<std::array<int, 3>> vertex_indices;
    std::vector<std::array<int, 3>> normal_indices;
    std::vector<std::array<int, 3>> texcoord_indices;
    int material_ids;
    std::string material_name;
};

struct Mesh {
    std::vector<Point3d> vertices;
    std::vector<Vector3d> normals;
    std::vector<Vector2d> texcoords;
    std::vector<Face_indices> faces;
    std::unordered_map<std::string, Material*> materialCache;
    std::unordered_map<std::string, Light*> lightCache;
    std::vector<Lightinfo> lightinfos;
};

class SceneLoader {
public:
    SceneLoader() = default;

    bool loader_xml(const std::string& xml_file, const std::string& basepath);
    bool loader_obj(const std::string& filename, const std::string& basepath, Mesh& mesh);
    
    const Camera& getCamera() const;
    const std::vector<std::shared_ptr<Light>>& getLights() const;
    const Mesh& getMeshes() const;

    void store(TriangleMesh* triangle_mesh);

    Camera camera;
    std::vector<std::shared_ptr<Light>> lights;
    std::vector<std::shared_ptr<Shape>> shapes;
    Mesh mesh;
};
