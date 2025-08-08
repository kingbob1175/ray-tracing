#pragma once

#include "geometry/geometry.hpp"
#include "tiny_obj_loader.h"
#include "base_material.hpp"
#include "texture/texture.hpp"
#include <string>

class Material {
public:
    Vector3d albedo;
    Vector3d specular;
    Vector3d Tf;
    float shiness;
    float Ni;
    float roughness;
    bool isdelta = false;

    std::string name;
    MaterialType type;

    Texture* albedoTexture=nullptr;
    Texture* normalTexture=nullptr;
    Texture* roughnessTexture=nullptr;

    void setAlbedoTexture(Texture* tex) { albedoTexture = tex; }
    void setNormalTexture(Texture* tex) { normalTexture = tex; }
    void setRoughnessTexture(Texture* tex) { roughnessTexture = tex; }

    virtual ~Material() = default;

    virtual Vector3d sample(const Vector3d& wo, const Vector3d& normal, const Vector3d& albedo, const float& rough) const = 0;
    virtual float pdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& albedo, const float& rough) const = 0;
    virtual Vector3d computeBSDF(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& albedo, const float& rough) const = 0;
    virtual Vector3d getAlbedo(const Vector2d& uv) const = 0;
    virtual float getRoughness(const Vector2d& uv) const = 0;
    bool hastexture() const {
        return albedoTexture != nullptr || roughnessTexture != nullptr || normalTexture != nullptr;
    }
};


// #pragma once

// #include "vector.hpp"
// #include "tiny_obj_loader.h"
// #include "math.hpp"

// enum MaterialType{
//     DIFFUSE,
//     MICROFACET,
//     MIRROR
// };

// Vector3d reflect(const Vector3d& wi, const Vector3d& normal){
//     return 2 * normal.dot(wi) * normal - wi;
// }

// // compute the Fresnel reflectance
// float Fresnel(const Vector3d& wi, const Vector3d& normal, float ior){
//     float cosi = std::clamp(normal.dot(wi), -1.0f, 1.0f);
//     float etai = 1.0f, etat = ior;
//     if (cosi < 0) {std::swap(etai, etat);}
//     cosi = std::abs(cosi);
//     float R0 = pow((etai-etat)/(etai+etat), 2);
//     return R0 + (1-R0)*pow((1-cosi),5);
// }

// // compute the GGX distribution
// float GGX_D(const Vector3d& h, const Vector3d& normal, float alpha){
//     float cosh = std::max(0.0f, h.dot(normal));
//     float alpha2 = alpha*alpha;
//     float denom = (cosh*cosh) * (alpha2-1.0f) + 1.0f;
//     return alpha2 / (PI * denom*denom);
// }

// float GGX_G(const Vector3d& normal, const Vector3d& wo, const Vector3d& wi, float alpha){
//     float cosi = wi.dot(normal);
//     float coso = wo.dot(normal);    
//     float alpha2 = alpha * alpha;
//     float lambda_i = (-1.0f + std::sqrt(1.0f+alpha2*(1.0f/cosi*cosi-1.0f))) / 2.0f;
//     float lambda_o = (-1.0f + std::sqrt(1.0f+alpha2*(1.0f/coso*coso-1.0f))) / 2.0f;
//     return 1.0f / (1.0f + lambda_i + lambda_o);
// }


// Vector3d sample_diffuse(const Vector3d& wo, const Vector3d& normal){
//     float r1 = get_random_number();
//     float r2 = get_random_number();
//     float r = sqrt(r1);
//     float theta = 2.0f * PI * r2;

//     float x = r * cos(theta);
//     float y = r * sin(theta);
//     float z = sqrt(1.0f - r1);

//     Vector3d wi_local = Vector3d(x, y, z);  
//     Vector3d wi_world = local_to_world(wi_local, normal).normalize();
//     return wi_world;
// }

// Vector3d sample_microfacet(const Vector3d& wo, float alpha, const Vector3d& normal){
//     float x1 = get_random_number();
//     float x2 = get_random_number() * 2.0f * PI;

//     // sample microfacet normal not wi
//     float theta = std::atan(alpha * sqrt(x1) / sqrt(1 - x1));
//     Vector3d h = Vector3d(std::sin(theta)*std::cos(x2), std::sin(theta)*std::sin(x2), std::cos(theta));
//     Vector3d h_world = local_to_world(h, normal).normalize();

//     return reflect(wo, h_world);
// }

// Vector3d sample_mirror(const Vector3d& wo, const Vector3d& normal){
//     return reflect(wo, normal);
// }

// float pdf_diffuse(){
//     return 1.0f / (2 * PI);
// }

// float pdf_microfacet(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, float alpha){
//     Vector3d h = (wo + wi).normalize();
//     float D = GGX_D(h, normal, alpha);
//     return D * (normal.dot(h)) / (4 * wi.dot(h));
// }

// float pdf_mirror(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal){
//     return 1.0f;
// }

// Vector3d computeBSDF_Diffuse(const Vector3d& albedo){
//     return albedo / PI;
// }

// Vector3d computeBSDF_microfacet(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& albedo, 
//     float alpha, float ior){
//     if(wo.dot(normal) < 0) return Vector3d(0, 0, 0);

//     Vector3d h = (wo + wi).normalize();

//     float D = GGX_D(h, normal, alpha);
//     float F = Fresnel(wi, normal, ior);
//     float G = GGX_G(normal, wo, wi, alpha);

//     Vector3d diffuse = (1.0f - F) * albedo / PI;
//     Vector3d specular = Vector3d(F) * D * G / (4 * wi.dot(h)) / (4 * wo.dot(h));

//     return diffuse + specular;
// }

// Vector3d computeBSDF_mirror(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, float ior){
//     float cosalpha = wo.dot(normal);
//     if(cosalpha > Epsilon) {
//         float F = Fresnel(wi, normal, ior);
//         return Vector3d(F) / cosalpha;
//     }else{
//         return Vector3d(0.0f);
//     }
// }

// class Material{
// public:
//     MaterialType type;

//     Vector3d albedo; // the diffuse reflectance of material
//     Vector3d specular; // the specular reflectance of material
//     Vector3d Tf; // the transmittance of material
//     float shiness; // shiness, the exponent of phong lobe
//     float Ni; // Index of Refraction(IOR)
//     float alpha; // the roughness of material

//     Material(const tinyobj::material_t material){
//         albedo = Vector3d(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
//         specular = Vector3d(material.specular[0], material.specular[1], material.specular[2]);
//         Tf = Vector3d(material.transmittance[0], material.transmittance[1], material.transmittance[2]);
//         shiness = material.shininess;
//         Ni = material.ior;
//         alpha = material.roughness * material.roughness;
//         // alpha = 2.0f / std::sqrt(2.0f + shiness)

//         if (specular.norm() < 1e-4) {
//             type = DIFFUSE;
//         }
//         else if (shiness >= 1e4) {
//             type = MIRROR; 
//         }
//         else {
//             type = MICROFACET;
//         }
//     }

//     // sample a direction wi
//     Vector3d sample(const Vector3d& wo, const Vector3d& p, const Vector3d& normal) const{
//         switch(type) {
//             case DIFFUSE:
//                 return sample_diffuse(wo, normal);
//                 break;
//             case MICROFACET:
//                 return sample_microfacet(wo, alpha, normal);
//                 break;
//             case MIRROR:
//                 return sample_mirror(wo, normal);
//                 break;
//         }
//     }

//     // compute pdf of wi
//     float pdf(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& p) const{
//         switch(type) {
//             case DIFFUSE:
//                 return pdf_diffuse();
//                 break;
//             case MICROFACET:
//                 return pdf_microfacet(wo, wi, normal, alpha);
//                 break;
//             case MIRROR:
//                 return pdf_mirror(wo, wi, normal);
//                 break;
//         }
//     }

//     // compute the BSDF
//     Vector3d computeBSDF(const Vector3d& wo, const Vector3d& wi, const Vector3d& normal, const Vector3d& p) const{
//         switch(type) {
//             case DIFFUSE:
//                 return computeBSDF_Diffuse(albedo);
//                 break;
//             case MICROFACET:
//                 return computeBSDF_microfacet(wo, wi, normal, albedo, alpha, Ni);
//                 break;
//             case MIRROR:
//                 return computeBSDF_mirror(wo, wi, normal, Ni);
//                 break;
//         }
//     }  
// };