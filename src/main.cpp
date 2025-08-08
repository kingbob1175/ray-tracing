#include <iostream>
#include <chrono>
#include "geometry/geometry.hpp"
#include "loader.hpp"
#include "basicmath/math.hpp"
#include "shape/triangle.hpp"
#include "material/mat.hpp"
#include "light/arealight.hpp"
#include "ray/ray.hpp"
#include "shape/intersection.hpp"
#include "camera/camera.hpp"
#include "scene.hpp"
#include "renderer.hpp"

int main(){
    
    SceneLoader loader;

    loader.loader_xml("D:/code/raytracing/example/bathroom2/bathroom2.xml", "D:/code/raytracing/example/bathroom2/");
    loader.loader_obj("D:/code/raytracing/example/bathroom2/bathroom2.obj", "D:/code/raytracing/example/bathroom2/", loader.mesh);
    
    TriangleMesh* triangle_mesh = new TriangleMesh(loader.mesh);

    loader.store(triangle_mesh);

    Scene scene;

    scene.add_shape(loader.shapes);
    scene.add_light(loader.lights);
    scene.InitLightCDF();

    scene.buildbvh(8, BVHBuildMethod::MIDDLE);

    Renderer r;

    auto start = std::chrono::system_clock::now();
    r.Render(scene, loader.camera);
    auto end = std::chrono::system_clock::now();

    std::cout << "Render complete: \n" << std::endl;
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(end - start).count() << "h" << std::endl;
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(end - start).count() << "min" << std::endl;
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "s" << std::endl;
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    // std::cout << "Intersection time: " << scene.time_intersection.count() << "s" << std::endl;
    // std::cout << "Light sampling time: " << scene.time_lightsampling.count() << "s" << std::endl;
    // std::cout << "maxDepth: " << scene.bvh->maxVisitedLeafDepth << std::endl;
    // std::cout << "minDepth: " << scene.bvh->minVisitedLeafDepth << std::endl;
    // std::cout << "average depth in bvh: " << float(scene.bvh->visitedLeafDepthSum) / scene.bvh->visitedLeafCount << std::endl;
    // std::cout << "number of visited leafs: " << scene.bvh->visitedLeafCount << std::endl;
    // std::cout << "depth sum: " << scene.bvh->visitedLeafDepthSum << std::endl;
    // std::cout << "triangle_intersection time in bvh: " << scene.bvh->time_intersection.count() << "s" << std::endl;
    
    return 0;
}
