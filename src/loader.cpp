#include "tiny_obj_loader.h"
#include "tinyxml2.h"
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include "geometry/geometry.hpp"
#include "material/mat.hpp"
#include "camera/camera.hpp"
#include "light/light.hpp"
#include "shape/shape.hpp"
#include "shape/triangle.hpp"
#include "light/arealight.hpp"
#include "loader.hpp"

bool SceneLoader::loader_xml(const std::string& xml_file, const std::string& basepath) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(xml_file.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error: cannot load scene file: " << xml_file << std::endl;
        return false;
    }

    // --- Load Camera ---
    tinyxml2::XMLElement* camera_elem = doc.FirstChildElement("camera");
    if (camera_elem) {
        camera.height = camera_elem->IntAttribute("height");
        camera.width = camera_elem->IntAttribute("width");
        camera.fov = camera_elem->FloatAttribute("fovy");

        tinyxml2::XMLElement* eye = camera_elem->FirstChildElement("eye");
        camera.eye = { eye->FloatAttribute("x"), eye->FloatAttribute("y"), eye->FloatAttribute("z") };

        tinyxml2::XMLElement* lookat = camera_elem->FirstChildElement("lookat");
        camera.target = { lookat->FloatAttribute("x"), lookat->FloatAttribute("y"), lookat->FloatAttribute("z") };

        tinyxml2::XMLElement* up = camera_elem->FirstChildElement("up");
        camera.up = { up->FloatAttribute("x"), up->FloatAttribute("y"), up->FloatAttribute("z") };

        // std::cout << "Camera: " << camera.eye << " -> " << camera.target << std::endl;
        // std::cout << "Camera up vector: " << camera.up << std::endl;
        // std::cout << "Camera fov: " << camera.fov << std::endl;
        // std::cout << "Camera width: " << camera.width << std::endl;
        // std::cout << "Camera height: " << camera.height << std::endl;
    }

    // --- Load Lights ---
    for (tinyxml2::XMLElement* light_elem = doc.FirstChildElement("light"); light_elem; 
        light_elem = light_elem->NextSiblingElement("light")) 
    {
        Lightinfo info;
        info.name = light_elem->Attribute("mtlname");

        const char* rad_str = light_elem->Attribute("radiance");
        float x = 0, y = 0, z = 0;
        if (rad_str) {
            sscanf_s(rad_str, "%f,%f,%f", &x, &y, &z);
        }
        info.radiance = Vector3d(x, y, z);

        mesh.lightinfos.push_back(info);

        // std::cout << "Light: " << info.name << " radiance: " << info.radiance << std::endl;
    }

    return true;
}

bool SceneLoader::loader_obj(const std::string& filename, const std::string& basepath, Mesh& mesh)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), basepath.c_str());

    if (!warn.empty()) std::cout << "Warn: " << warn << std::endl;
    if (!err.empty()) std::cerr << "Err: " << err << std::endl;

    if (!ret) {
        std::cerr << "Failed to load .obj" << std::endl;
        return false;
    }
 
    for (size_t i = 0; i < attrib.vertices.size(); i+=3) {
        mesh.vertices.push_back(Point3d(attrib.vertices[i], attrib.vertices[i+1], attrib.vertices[i+2]));
    }

    for (size_t i = 0; i < attrib.normals.size(); i+=3) {
        mesh.normals.push_back(Vector3d(attrib.normals[i], attrib.normals[i+1], attrib.normals[i+2]));
    }

    for (size_t i = 0; i < attrib.texcoords.size(); i+=2) {
        mesh.texcoords.push_back(Vector2d(attrib.texcoords[i], attrib.texcoords[i+1]));
    }

    for (auto& mat : materials) {
        Material* material = nullptr;
        Light* light = nullptr;
        for (const auto& info : mesh.lightinfos) {
            if (info.name == mat.name) {
                light = new AreaLight(info.radiance, mat.name);
                mesh.lightCache[mat.name] = light;
                std::cout << "light_name = " << light->name << std::endl;
                break;
            }
        }
        if (light == nullptr) {
            material = CreateMaterial(mat, mat.name, basepath);
            mesh.materialCache[mat.name] = material;
            std::cout << "material_name = " << material->name << " type: " << material->type << std::endl;
            std::cout << "albedo: " << material->albedo << std::endl;
            std::cout << "roughness: " << material->roughness << std::endl; 
            std::cout << "whether it has texture: " << material->hastexture() << std::endl;
        }
    }

    // for (size_t i = 0; i < materials.size(); i++) {
    //     mesh.mat.push_back(materials[i]);
    // }
    // std::cout << "number of materials:" << materials.size() << std::endl;

    // std::cout << "number of shapes: " << shapes.size() << std::endl;

    for (const auto& shape : shapes) { // loop over objects/groups
        size_t index_offset = 0;
        std::cout << "number of faces: " << shape.mesh.num_face_vertices.size() << std::endl;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) { // loop over face
            int fv = shape.mesh.num_face_vertices[f];

            Face_indices face;
            face.material_ids = shape.mesh.material_ids[f];
            face.material_name = materials[face.material_ids].name;

            for (int v = 0; v < fv; v = v + 3) {
                const tinyobj::index_t& idx0 = shape.mesh.indices[index_offset + v];
                const tinyobj::index_t& idx1 = shape.mesh.indices[index_offset + v + 1];
                const tinyobj::index_t& idx2 = shape.mesh.indices[index_offset + v + 2];
                face.vertex_indices.push_back({ idx0.vertex_index, idx1.vertex_index, idx2.vertex_index });
                face.normal_indices.push_back({ idx0.normal_index, idx1.normal_index, idx2.normal_index });
                face.texcoord_indices.push_back({ idx0.texcoord_index, idx1.texcoord_index, idx2.texcoord_index });
            }

            mesh.faces.push_back(face);
            index_offset += fv;
        }
    }

    std::sort(mesh.faces.begin(), mesh.faces.end(), [](const Face_indices& a, const Face_indices& b) {
        return a.material_ids < b.material_ids;
    });

    return true;
}


const Camera& SceneLoader::getCamera() const {
    return camera;
}

const std::vector<std::shared_ptr<Light>>& SceneLoader::getLights() const {
    return lights;
}

const Mesh& SceneLoader::getMeshes() const {
    return mesh;
}

void SceneLoader::store(TriangleMesh* triangle_mesh) {

    for (const auto& tri : triangle_mesh->triangles) {
        shapes.push_back(std::shared_ptr<Shape>(tri));
        if (tri->light) {
            auto light = new AreaLight(tri, tri->light->radiance);
            tri->light = light;
            lights.push_back(std::shared_ptr<Light>(tri->light));
        }
    }

    std::cout << "number of emissive triangles: " << lights.size() << std::endl;
}