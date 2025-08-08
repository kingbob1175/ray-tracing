
#pragma once
#include "texture.hpp"
#include <unordered_map>
#include <string>

class TextureManager {
public:
    static TextureManager& instance() {
        static TextureManager tm;
        return tm;
    }
    
    Texture* getTexture(const std::string& path) {
        if(path.empty()) return nullptr;
        
        auto it = cache.find(path);
        if(it != cache.end()) return it->second;
        
        Texture* tex = loadTexture(path);
        cache[path] = tex;
        return tex;
    }
    
private:
    Texture* loadTexture(const std::string& path) {
        return new ImageTexture(path);
    }
    
    std::unordered_map<std::string, Texture*> cache;
};