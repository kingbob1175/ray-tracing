#pragma once

#include "geometry/geometry.hpp"
#include "basicmath/math.hpp"
#include <string>
#include <memory>

class Texture {
public:
    virtual ~Texture() = default;

    virtual Vector3d sample(const Vector2d& uv) const = 0;
};

// PURE COLOR
class SolidColor: public Texture {
public:
    SolidColor(const Vector3d& color) : color(color) {}
    Vector3d sample(const Vector2d& uv) const override { return color; }

private:
    Vector3d color;
};

class ImageTexture: public Texture {
public:
    ImageTexture(const std::string& filename);
    Vector3d sample(const Vector2d& uv) const override;

private:
    int width, height;
    std::unique_ptr<Vector3d[]> data;
};


class CheckerTexture: public Texture {
public:
    CheckerTexture(float scale, const Vector3d& color1, const Vector3d& color2):
        scale(scale), color1(color1), color2(color2) {}

    Vector3d sample(const Vector2d& uv) const override;

private:
    float scale;
    Vector3d color1, color2;
};