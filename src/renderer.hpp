#pragma once

#include "scene.hpp"
#include "camera/camera.hpp"

class Renderer {
public:
    void Render(const Scene& scene, Camera camera);
};
