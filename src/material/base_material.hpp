#pragma once

#include "geometry/geometry.hpp"
#include "basicmath/math.hpp"

enum MaterialType {
    DIFFUSE,
    MICROFACET,
    MIRROR,
    TRANSPARENT
};

inline Vector3d reflect(const Vector3d& wi, const Vector3d& normal){
    return 2 * normal.dot(wi) * normal - wi;
}

inline Vector3d refract(const Vector3d& wi, const Vector3d& normal, float eta) {
    float cosi = std::clamp(normal.dot(wi), -1.0f, 1.0f);
    float cost = std::sqrt(1.0f - eta*eta*(1.0f - cosi*cosi));
    return (-eta*wi + (eta*cosi - cost)*normal).normalize();
}

// compute the Fresnel reflectance
inline Vector3d Fresnel(const Vector3d& wi, const Vector3d& normal, float ior){
    float cosi = std::clamp(normal.dot(wi), -1.0f, 1.0f);
    float etai = 1.0f, etat = ior;
    if (cosi < 0) {std::swap(etai, etat);} 
    cosi = std::abs(cosi);
    float R0 = pow((etai-etat)/(etai+etat), 2.0f); 
    return Vector3d(R0 + (1.0f-R0)*pow((1.0f-cosi), 5.0f));
}

inline Vector3d Fresnel(const Vector3d& wi, const Vector3d& normal, Vector3d R0){
    float cosi = std::clamp(normal.dot(wi), -1.0f, 1.0f);
    cosi = std::abs(cosi);
    return R0 + (1.0f-R0)*pow((1.0f-cosi), 5.0f);
}


// compute the GGX normal distribution function
inline float GGX_D(const Vector3d& h, const Vector3d& normal, float alpha){
    float cosh = std::max(0.0f, h.dot(normal));
    float alpha2 = alpha*alpha;
    float denom = (cosh*cosh) * (alpha2-1.0f) + 1.0f;
    return alpha2 / (PI * denom*denom);
}

inline float GGX_G1(const Vector3d& normal, const Vector3d& wi, float alpha){
    float cosi = wi.dot(normal);
    if (cosi < Epsilon) return 0.0f;    

    float alpha2 = alpha * alpha; 
    float lambda_i = (-1.0f + std::sqrt(1.0f+alpha2*(1.0f/(cosi*cosi)-1.0f))) / 2.0f;
    return 1.0f / (1.0f + lambda_i);
}

inline float GGX_G(const Vector3d& normal, const Vector3d& wo, const Vector3d& wi, float alpha){
    return GGX_G1(normal, wi, alpha) * GGX_G1(normal, wo, alpha);
}

// compute the Beckmann microfacet distribution function
inline float Beckmann_D(const Vector3d& h, const Vector3d& normal, float alpha){
    float cosh = std::max(0.0f, h.dot(normal));
    float alpha2 = alpha*alpha;
    float cos_4_thetah = cosh*cosh*cosh*cosh;
    float tan_2_thetah = 1 / (cosh*cosh) - 1;
    return expf(-tan_2_thetah / alpha2) / (PI * alpha2 * cos_4_thetah);
}

inline float Beckmann_G1(const Vector3d& normal, const Vector3d& wi, float alpha){
    float cosi = wi.dot(normal);
    float sini = std::sqrt(1.0f - cosi * cosi);
    float tani = sini / cosi;
    if (cosi < Epsilon) return 0.0f;  

    float a = 1.0f / (alpha * tani);
    if (a >= 1.6f) return 1.0f;
    /* Use a fast and accurate (<0.35% rel. error) rational
        approximation to the shadowing-masking function */
    float aSqr = a*a;
    return (3.535f * a + 2.181f * aSqr) / (1.0f + 2.276f * a + 2.577f * aSqr);
}

inline float Beckmann_G(const Vector3d& normal, const Vector3d& wo, const Vector3d& wi, float alpha){
    return Beckmann_G1(normal, wi, alpha) * Beckmann_G1(normal, wo, alpha);
}

inline Vector3d local_to_world(const Vector3d& wi, const Vector3d& Normal) {
    Vector3d nx, ny;
    Vector3d t = (std::abs(Normal.x) > 0.99) ? Vector3d(0, 1, 0) : Vector3d(1, 0, 0);

    nx = Normal.cross(t).normalize();
    ny = Normal.cross(nx);

    return wi.x * nx + wi.y * ny + wi.z * Normal;
} 