#pragma once

#include "base_material.hpp"
#include "material.hpp"

class DiffuseMaterial : public Material {
public:
    DiffuseMaterial(const tinyobj::material_t& material, const std::string& _name) {
        albedo = Vector3d(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        name = _name;
        type = MaterialType::DIFFUSE;
    }

    DiffuseMaterial() {
        albedo = Vector3d(0.0f, 0.0f, 0.0f);
        type = MaterialType::DIFFUSE;
    }

    Vector3d getAlbedo(const Vector2d& uv) const override {
        if (albedoTexture) {
            return albedoTexture->sample(uv);
        }
        return albedo;
    }

    float getRoughness(const Vector2d& uv) const override {
        if (roughnessTexture) {
            Vector3d roughness = roughnessTexture->sample(uv);
            return (roughness.x + roughness.y + roughness.z) / 3.0f;
        }
        return roughness;
    }

    Vector3d sample(const Vector3d& wo, const Vector3d& normal, const Vector3d& my_albedo, const float& rough) const override {   
        float r1 = get_random_number();
        float r2 = get_random_number();
        float r = sqrt(r1);
        float theta = 2.0f * PI * r2;

        float x = r * cos(theta);
        float y = r * sin(theta);
        float z = sqrt(1.0f - r1);

        Vector3d wi_local = Vector3d(x, y, z);  
        Vector3d wi_world = local_to_world(wi_local, normal).normalize();
        
        return wi_world;
    }

    float pdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& my_albedo, const float& rough) const override {
        // return 1.0f / (2 * PI);
        float cosTheta = std::max(wi.dot(normal), 0.0f);
        return cosTheta / PI; 
    }

    Vector3d computeBSDF(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& my_albedo, const float& rough) const override {
        if (my_albedo.x == 0.0f && my_albedo.y == 0.0f && my_albedo.z == 0.0f) {
            std::cerr << "Warning: albedo is zero in DiffuseMaterial::computeBSDF()" << std::endl;
        }
        return my_albedo / PI;
    }
};
