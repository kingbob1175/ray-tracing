
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture/texture.hpp"

ImageTexture::ImageTexture(const std::string& filename) {
    int channels;
    unsigned char* img_data = stbi_load(filename.c_str(), &width, &height, &channels, 3);
    
    if (!img_data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        width = height = 1;
        data = std::make_unique<Vector3d[]>(1);
        data[0] = Vector3d(1.0f, 0.0f, 1.0f); 
        return;
    }

    data = std::make_unique<Vector3d[]>(width * height);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            int img_idx = (y * width + x) * 3;
            data[idx] = Vector3d(
                img_data[img_idx] / 255.0f,
                img_data[img_idx + 1] / 255.0f,
                img_data[img_idx + 2] / 255.0f
            );
        }
    }
    stbi_image_free(img_data);
}


Vector3d ImageTexture::sample(const Vector2d& uv) const {
    float u = uv.x - floor(uv.x);
    float v = 1 - (uv.y - floor(uv.y));

    float x = u * width;
    float y = v * height;

    float x_floor = std::floor(x - 0.5f);
    float y_floor = std::floor(y - 0.5f);
    int x0 = static_cast<int>(x_floor) % width;
    int y0 = static_cast<int>(y_floor) % height;
    if (x0 < 0) x0 += width;
    if (y0 < 0) y0 += height;
    int x1 = (x0 + 1) % width;
    int y1 = (y0 + 1) % height;

    float dx = x - (x_floor + 0.5f);
    float dy = y - (y_floor + 0.5f);

    Vector3d u00 = data[y0*width + x0];
    Vector3d u10 = data[y0*width + x1];
    Vector3d u01 = data[y1*width + x0];
    Vector3d u11 = data[y1*width + x1];

    return lerp(dy, lerp(dx, u00, u10), lerp(dx, u01, u11));
}

Vector3d CheckerTexture::sample(const Vector2d& uv) const {
    float s = uv.x * scale;
    float t = uv.y * scale;
    int pattern = (static_cast<int>(floor(s)) + static_cast<int>(floor(t))) % 2;
    return pattern ? color1 : color2;
}