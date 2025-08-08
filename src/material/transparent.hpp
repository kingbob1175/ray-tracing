#pragma once

#include "base_material.hpp"
#include "material.hpp"
#include <algorithm>

class TransparentMaterial: public Material {
public:
    TransparentMaterial(){
        Ni = MY_INFINITY;
        type = MaterialType::TRANSPARENT;
    }

    TransparentMaterial(const tinyobj::material_t& material, const std::string& _name) {
        Ni = material.ior;
        name = _name;
        type = MaterialType::TRANSPARENT;
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
        if (Ni == MY_INFINITY) {
            std::cerr << "Warning: ior is not initialized in TransparentMaterial::sample()" << std::endl;
        }
        
        float cosi = std::clamp(wo.dot(normal), -1.0f, 1.0f);
        float etai = 1.0f, etat = Ni;
        Vector3d n = normal;
        Vector3d F = Fresnel(wo, n, Ni);
        // std::cout << "Fresnel: " << F << std::endl;

        if (cosi < 0.0f) {
            std::swap(etai, etat);
            n = -n;
        }
        cosi = std::abs(cosi);

        float eta = etai / etat;
        float sint2 = eta * eta * (1.0f - cosi * cosi);
        if (sint2 > 1.0f) { // total reflection
            return reflect(wo, n);
        }

        if(get_random_number() < F.x){
            std::cout << "reflection" << std::endl;
            return reflect(wo, n);
        }else{
            std::cout << "refraction" << std::endl;
            return refract(wo, n, eta);
        }
    }

    float pdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& my_albedo, const float& rough) const override {
        return 1.0f;
    }

    Vector3d computeBSDF(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& my_albedo, const float& rough) const override {        
        if (Ni == MY_INFINITY) {
            std::cerr << "Warning: ior is not initialized in TransparentMaterial::computeBSDF()" << std::endl;
        }
        
        float cosi = std::clamp(wo.dot(normal), -1.0f, 1.0f);
        float etai = 1.0f, etat = Ni;
        Vector3d n = normal;
        Vector3d F = Fresnel(wo, n, Ni);

        if (cosi < 0.0f) {
            std::swap(etai, etat);
            n = -n;
        }
        cosi = std::abs(cosi);

        float eta = etai / etat;
        float sint2 = eta * eta * (1.0f - cosi * cosi);
        if (sint2 > 1.0f) { // total reflection
            return F / std::abs(normal.dot(wi));
        }

        bool is_reflection = (wi - reflect(wo, normal)).norm() < 1e-3;

        if (is_reflection) {
            return F / std::abs(normal.dot(wi));
        } else {
            float transmittance = 1.0f - F.x;
            float scale = (etai * etai) / (etat * etat);
            return Vector3d(transmittance * scale) / std::abs(normal.dot(wi));
        }
    }
};