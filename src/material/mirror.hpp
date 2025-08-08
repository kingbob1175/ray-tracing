#pragma once

#include "base_material.hpp"
#include "material.hpp"

class MirrorMaterial : public Material {
public:
    MirrorMaterial(const tinyobj::material_t& material, const std::string& _name) {
        albedo = Vector3d(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        name = _name;
        type = MaterialType::MIRROR;
        isdelta = true;
    }

    MirrorMaterial() {
        albedo = Vector3d(1.0f, 1.0f, 1.0f);
        type = MaterialType::MIRROR;
        isdelta = true;
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
        return reflect(wo, normal);
    }

    float pdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& my_albedo, const float& rough) const override {
        return 1.0f;
    }

    Vector3d computeBSDF(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& my_albedo, const float& rough) const override {        
        Vector3d reflectDir = reflect(wo, normal);
        if ((wi - reflectDir).norm() < 1e-3f) {
            float cosTheta = std::abs(normal.dot(wi));
            return 1.0f / cosTheta;  
        } else {
            return Vector3d(0.0f);
        }
    }
};