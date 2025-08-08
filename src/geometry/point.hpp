#pragma once

#include <iostream>
#include <cmath>
#include <algorithm>
#include "vector.hpp"

class Point3d {
public:
    float x, y, z;

    Point3d(): x(0), y(0), z(0) {}
    Point3d(float _x, float _y, float _z): x(_x), y(_y), z(_z) {}
    Point3d(const Point3d& p): x(p.x), y(p.y), z(p.z) {}
    Point3d(float xx):x(xx), y(xx), z(xx) {}

    float getX() const { return x;} 
    float getY() const { return y;}
    float getZ() const { return z;}

    friend std::ostream& operator<<(std::ostream& os, const Point3d& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

    bool operator==(const Point3d& v) const {return x==v.x && y==v.y && z==v.z;}

    Point3d operator+(const Point3d& v) const {return Point3d(x+v.x, y+v.y, z+v.z);}
    Point3d operator+(const Vector3d& v) const {return Point3d(x+v.x, y+v.y, z+v.z);}
    Point3d operator+=(const Vector3d& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Point3d operator-(const Vector3d& v) const {return Point3d(x-v.x, y-v.y, z-v.z);}
    Point3d operator-=(const Vector3d& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    Vector3d operator-(const Point3d& p) const {return Vector3d(x-p.x, y-p.y, z-p.z);}  

    Point3d operator*(float s) const {return Point3d(x*s, y*s, z*s);}
    friend Point3d operator*(const float s, const Point3d& v) {return Point3d(v.x*s, v.y*s, v.z*s);}

    float& operator[](size_t i) {
        return *(&x + i);
    }

    const float& operator[](size_t i) const {
        return *(&x + i);
    }

    Point3d floor() const {
        return Point3d(std::floor(x), std::floor(y), std::floor(z));
    }
    Point3d ceil() const {
        return Point3d(std::ceil(x), std::ceil(y), std::ceil(z));
    }
    Point3d abs() const {
        return Point3d(std::abs(x), std::abs(y), std::abs(z));
    }

    float maxdir() const {
        return std::abs(std::max(std::max(x, y), z));
    }
};

inline float distance(const Point3d& p1, const Point3d& p2) {
    return (p1 - p2).norm();
}

inline float distance2(const Point3d& p1, const Point3d& p2) {
    return (p1 - p2).norm2();
}

inline Point3d operator+(const Vector3d& v, const Point3d &p) {
    return Point3d(v.x + p.x, v.y + p.y, v.z + p.z);
}

inline Vector3d operator-(const Point3d& p, const Vector3d& v) {
    return Vector3d(p.x - v.x, p.y - v.y, p.z - v.z);   
}

static Point3d Max(const Point3d& v1, const Point3d& v2) {
    return Point3d(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
}
static Point3d Min(const Point3d& v1, const Point3d& v2) {
    return Point3d(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
}