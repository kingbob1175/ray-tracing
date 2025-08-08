#pragma once

#include "base_material.hpp"
#include "material.hpp"


class MicrofacetMaterial : public Material {
public:

    MicrofacetMaterial(const tinyobj::material_t& material, const std::string& _name) {
        albedo = Vector3d(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        specular = Vector3d(material.specular[0], material.specular[1], material.specular[2]);
        Tf = Vector3d(material.transmittance[0], material.transmittance[1], material.transmittance[2]);
        shiness = material.shininess;
        Ni = material.ior;
        // alpha = std::max(0.001f, material.roughness * material.roughness);
        // roughness = material.roughness;
        roughness = 2.0f / std::sqrt(shiness + 2.0f);

        name = _name;
        type = MaterialType::MICROFACET;
    }

    MicrofacetMaterial(){
        albedo = Vector3d(0.0f, 0.0f, 0.0f);
        specular = Vector3d(0.0f, 0.0f, 0.0f);
        Tf = Vector3d(0, 0, 0);
        shiness = 0.0f;
        Ni = 1.0f;
        roughness = MY_INFINITY; 
        type = MaterialType::MICROFACET;
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

    Vector3d sample(const Vector3d& wo, const Vector3d& normal, 
                    const Vector3d& my_albedo, const float& rough) const override {
        float alpha = std::max(0.001f, pow(rough, 2.0f)); 
        float x1 = get_random_number();
        float x2 = get_random_number() * 2.0f * PI;

        /* ----- GGX ----- */
        float cos_theta = sqrt((1.0f - x1) / (1.0f + (alpha*alpha - 1.0f) * x1));
        float sin_theta = sqrt(std::max(0.0f, 1.0f - cos_theta*cos_theta));
        float phi = x2;

        Vector3d h_local(
                    sin_theta * cos(phi),
                    sin_theta * sin(phi),
                    cos_theta
                );

        // /* ----- Beckmann -----*/
        // float theta = std::atan(-alpha * alpha * log(1.0f - x1));
        // float phi = x2;

        // Vector3d h_local(
        //             std::sin(theta) * cos(phi),
        //             std::sin(theta) * sin(phi),
        //             std::cos(theta)
        //         );
        
        Vector3d h_world = local_to_world(h_local, normal).normalize();

        return reflect(wo, h_world).normalize();
    }

    float pdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& my_albedo, 
            const float& rough) const override {        
        float alpha = pow(rough, 2.0f);
        
        Vector3d h = (wo + wi).normalize();

        float D = GGX_D(h, normal, alpha);
        float n_dot_h = std::abs(normal.dot(h));
        float wi_dot_h = std::abs(wi.dot(h));

        if (wi_dot_h < 1e-6f) return 0.0f;
        
        return D * n_dot_h / (4.0f * wi_dot_h);
    }

    Vector3d computeBSDF(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& my_albedo, const float& rough) const override {
        float alpha = pow(rough, 2.0f);
        if(alpha == MY_INFINITY){
            std::cerr << "Warning: alpha is not initialized in MicrofacetMaterial" << std::endl;   
        }
        
        if (wo.dot(normal) <= 1e-6f) return Vector3d(0, 0, 0);
        if (wi.dot(normal) <= 1e-6f) return Vector3d(0, 0, 0);

        Vector3d h = (wo + wi).normalize();

        float D = GGX_D(h, normal, alpha);
        // Vector3d F = Fresnel(wo, normal, Ni);
        float G = GGX_G(normal, wo, wi, alpha);
        Vector3d F = Fresnel(wo, normal, specular);

            // ====== Kulla-Conty computation ======
        // 1. Average specular reflectance: E_avg
        float E_avg = 1.0f - (0.8638f * pow(alpha, 3.0f) - 1.8464f * pow(alpha, 2.0f) + 2.2598f * alpha)
                    / (pow(alpha, 3.0f) - 2.5769f * pow(alpha, 2.0f) + 2.5846f * alpha + 1.0f);

        // 2. Average directional reflectance: E(mu) (quadratic interpolation)
        auto compute_E = [](float mu, float E_avg) {
            return (1.0f - mu*mu) * E_avg + mu*mu;
        };
        
        float mu_o = std::abs(wo.dot(normal));
        float mu_i = std::abs(wi.dot(normal));
        float E_o = compute_E(mu_o, E_avg);
        float E_i = compute_E(mu_i, E_avg);

        // 3. Average Fresnel term: F_avg
        auto F_avg = [](float F0) {
            return (F0 * 20.0f / 21.0f) + (1.0f / 21.0f);
        };
        // Vector3d F_avg_vec(F_avg(Ni), F_avg(Ni), F_avg(Ni));
        Vector3d F_avg_vec(F_avg(specular.x), F_avg(specular.y), F_avg(specular.z));

        // 4. Compensate for scattering
        Vector3d f_add = F_avg_vec * F_avg_vec * E_avg / 
                        (Vector3d(1,1,1) - F_avg_vec * (1 - E_avg));

        // 5. Compensate for multiple scattering
        Vector3d f_ms = f_add * ((1 - E_o) * (1 - E_i)) / 
                        (PI * std::max(1 - E_avg, Epsilon));

        float cos_theta_incident = std::max(wi.dot(normal), 0.0f);
        Vector3d diffuse = (1 - F) * albedo / PI;
        Vector3d specu = F * D * G / std::max(4 * wi.dot(normal), Epsilon) / std::max(Epsilon, wo.dot(normal));

        Vector3d result = specu + f_ms;
        return result;
    }

};

