#pragma once

#include <cmath>
#include <iostream>
#include <random>
#include <limits>
#include <string>

#define PI 3.14159265358979323846f
#define Epsilon 0.000001f
#define MY_INFINITY std::numeric_limits<float>::infinity()

inline float degree_to_radian(float degree) {
    return degree * PI / 180.0f;
}

inline float radian_to_degree(float radian) {
    return radian * 180.0f / PI;
}

inline bool quadratic(const float a, const float b, const float c, float &r1, float &r2) {
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {return false;}
    r1 = (-b - std::sqrt(discriminant)) / (2 * a);
    r2 = (-b + std::sqrt(discriminant)) / (2 * a);
    if (r1 > r2) {std::swap(r1, r2);}
    return true;
}

// return a random float number between 0 and 1 
inline float get_random_number() {
    static std::random_device rd;
    static std::mt19937 rng(rd());
    static std::uniform_real_distribution<float> dist(0.f, 1.f);
    
    return dist(rng);
}

// linearly interpolate between a and b by t
inline float lerp(float t, float a, float b) {
    return (1 - t) * a + t * b;
}

template<typename Src, typename Dst>
inline std::vector<std::shared_ptr<Dst>> ptr_cast_vector(const std::vector<std::shared_ptr<Src>>& src) {
    std::vector<std::shared_ptr<Dst>> dst;
    dst.reserve(src.size());
    for (const auto& p : src)
        dst.push_back(std::static_pointer_cast<Dst>(p));
    return dst;
}

inline void Updateprinter(float progress) {
    int barWidth = 70;

    std::cout << "[";
    int pos = int(barWidth * progress);
    for (int i = 0; i < barWidth; i++) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0f) << " %\r";
    std::cout.flush();
}

inline float miWeight(float pdfA, float pdfB) {
    // pdfA *= pdfA; pdfB *= pdfB;
    return pdfA / (pdfA + pdfB);
}