cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)

project(raytracing)

# 设置默认构建类型为 Release（仅在未设置时）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /GL /fp:fast /DNDEBUG /Gw /Gy /Zc:inline /GF")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/LTCG /OPT:REF /OPT:ICF")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ot")
endif()

add_executable(raytracing 
    src/main.cpp 
    external/tiny_obj_loader.cc 
    external/tinyexr.cc 
    external/miniz.c 
    external/tinyxml2.cpp 
    src/acceleration/BVH.cpp 
    src/shape/triangle.cpp 
    src/light/arealight.cpp
    src/loader.cpp 
    src/scene.cpp
    src/renderer.cpp
    src/texture/texture.cpp
)

target_include_directories(raytracing PRIVATE 
    D:/code/raytracing/external 
    D:/code/raytracing/src
)