#include "ray/ray.hpp"
#include "scene.hpp"
#include "camera/camera.hpp"
#include "geometry/geometry.hpp"
#include "basicmath/math.hpp"
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include "renderer.hpp"
#include "tinyexr.h"

void Renderer::Render(const Scene& scene, const Camera camera) {

    int width = camera.width;
    int height = camera.height;

    int m = 0, spp = 512;

    std::vector<Vector3d> pixels(width * height);

    // auto start = std::chrono::system_clock::now();  

    // for (int j = 0; j < height; j++) {
    //      for (int i = 0; i < width; i++){
    //         Ray r = camera.generateRay(i, j);

    //         for (int k = 0; k < spp; k++) {
    //             pixels[m] += scene.Shade(r, 0) / (float)spp;
    //         }
    //         m++;
    //     }
    //     Updateprinter((j+1) / (float)height);
    // }
    // Updateprinter(1.f);

    // Ray r = camera.generateRay(900, 240);
    // pixels[m] += scene.Shade(r, 0) / (float)spp;

    int thread_num = 8;
    int thread_height = height / thread_num;
    std::vector<std::thread> threads(thread_num);
    std::cout << "SPP: " << spp << "\n";

    std::mutex mtx;
    float process=0, local_process=0;
    float Reciprocal_Scene_height=1.f/ height;
    int tile_size = 16;  
    int total_tiles = (width * height) / (tile_size * tile_size);
    auto castRay = [&](int thread_index) {
        for (int tile_y = thread_index * tile_size; tile_y < height; tile_y += thread_num * tile_size) {
            for (int tile_x = 0; tile_x < width; tile_x += tile_size) {
                for (int j = tile_y; j < std::min(tile_y + tile_size, height); ++j) {
                    for (int i = tile_x; i < std::min(tile_x + tile_size, width); ++i) {
                        Ray r = camera.generateRay(i, j);
                        for (int k = 0; k < spp; ++k) {
                            pixels[j * width + i] += scene.Shade(r, 0) / spp;
                        }
                    }
                }
                local_process += 1.f / (total_tiles);
                mtx.lock();
                process = process + local_process;
                Updateprinter(process);
                mtx.unlock();
                local_process = 0.f;
            }
        }
       
    };

    for (int k = 0; k < thread_num; k++) {
        threads[k] = std::thread(castRay, k);
    }
    for (int k = 0; k < thread_num; k++) {
        threads[k].join();
    }
    Updateprinter(1.f);

    // ----- Save image to file -----
    SaveEXR(reinterpret_cast<const float*>(pixels.data()), width, height, 3, 0, "image.exr", nullptr);

    FILE* file = nullptr;
    errno_t err = fopen_s(&file, "image.ppm", "wb");
    if (err != 0 || file == nullptr) {
        std::cerr << "Failed to open file for writing\n";
    } else {
        (void)fprintf(file, "P6\n%d %d\n255\n", width, height);
        for (auto i = 0; i < width * height; i++) {
            static unsigned char color[3];
            color[0] = (unsigned char)(255 * std::pow(std::clamp(pixels[i].x, 0.0f, 1.0f), 0.6f));
            color[1] = (unsigned char)(255 * std::pow(std::clamp(pixels[i].y, 0.0f, 1.0f), 0.6f));
            color[2] = (unsigned char)(255 * std::pow(std::clamp(pixels[i].z, 0.0f, 1.0f), 0.6f));
            fwrite(color, 1, 3, file);
        }
        fclose(file);
    }
    // auto end = std::chrono::system_clock::now();
    // std::cout << "render time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " s" << std::endl;
    // std::cout << "                 : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    // std::cout << "                 : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us" << std::endl;
}