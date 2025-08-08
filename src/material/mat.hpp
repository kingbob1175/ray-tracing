#pragma once

#include "material.hpp"
#include "diffuse.hpp"
#include "microfacet.hpp"
#include "mirror.hpp"
#include "transparent.hpp"
#include "base_material.hpp"
#include "texture/texturemanager.hpp"

inline void loadTexturesForMaterial(Material* mat, 
                            const tinyobj::material_t& material, 
                            const std::string& baseDir) 
{
    if(!mat) return;
    
    auto& tm = TextureManager::instance();
    
    if(!material.diffuse_texname.empty()) {
        std::string fullPath = baseDir + material.diffuse_texname;
        mat->setAlbedoTexture(tm.getTexture(fullPath));
    }
    
    if(!material.normal_texname.empty()) {
        std::string fullPath = baseDir + material.normal_texname;
        mat->setNormalTexture(tm.getTexture(fullPath));
    }
    
    if(!material.roughness_texname.empty()) {
        std::string fullPath = baseDir + material.roughness_texname;
        mat->setRoughnessTexture(tm.getTexture(fullPath));
    }
    
}

// Factory function
inline Material* CreateMaterial(const tinyobj::material_t& material, const std::string& name, const std::string& baseDir="") {
    Vector3d specular(material.specular[0], material.specular[1], material.specular[2]);
    float shiness = material.shininess;

    bool isDielectric = material.transmittance[0] > 0.1f;

    Material* mat = nullptr;

    if(isDielectric){
        mat = new TransparentMaterial(material, name);
    } else if (specular.norm() < 1e-4f) { 
        mat = new DiffuseMaterial(material, name);
    } else if (shiness >= 1e4f) {
        mat = new MirrorMaterial(material, name);
    } else {
        mat = new MicrofacetMaterial(material, name);
    } 

    loadTexturesForMaterial(mat, material, baseDir);

    return mat;
}

inline Material* CreateMaterial(MaterialType type){
    if (type == DIFFUSE) {
        return new DiffuseMaterial();
    } else if (type == MICROFACET) {
        return new MicrofacetMaterial();
    } else if (type == MIRROR) {
        return new MirrorMaterial();
    } else if (type == TRANSPARENT) {
        return new TransparentMaterial();
    } else {
        std::cerr << "Error: Invalid material type" << std::endl;
    }
}