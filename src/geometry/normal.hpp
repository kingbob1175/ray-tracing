#pragma once

#include <cmath>
#include <iostream>
#include <algorithm> 
#include "vector.hpp"

class Normal3d {
public:
    float x, y, z;

    Normal3d(): x(0), y(0), z(0) {}
    Normal3d(float _x, float _y, float _z): x(_x), y(_y), z(_z) {}
    Normal3d(float v): x(v), y(v), z(v) {}
    Normal3d(const Normal3d& n): x(n.x), y(n.y), z(n.z) {}

    // getter
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }

    Normal3d operator-() const { return Normal3d(-x, -y, -z); }

    Normal3d operator+(const Normal3d& n) const {
        return Normal3d(x + n.x, y + n.y, z + n.z);
    }

    Normal3d& operator+=(const Normal3d& n) {
        x += n.x; y += n.y; z += n.z;
        return *this;
    }

    Normal3d operator-(const Normal3d& n) const {
        return Normal3d(x - n.x, y - n.y, z - n.z);
    }

    Normal3d& operator-=(const Normal3d& n) {
        x -= n.x; y -= n.y; z -= n.z;
        return *this;
    }

    Normal3d operator*(float s) const {
        return Normal3d(x * s, y * s, z * s);
    }

    Normal3d& operator*=(float s) {
        x *= s; y *= s; z *= s;
        return *this;
    }

    Normal3d operator/(float s) const {
        float inv = 1.0f / s;
        return Normal3d(x * inv, y * inv, z * inv);
    }

    Normal3d& operator/=(float s) {
        float inv = 1.0f / s;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }

    bool operator==(const Normal3d& n) const {
        return x == n.x && y == n.y && z == n.z;
    }

    float norm() const {
        return std::sqrt(x*x + y*y + z*z);
    }

    float norm2() const {
        return x*x + y*y + z*z;
    }

    Normal3d normalized() const {
        return *this / norm();
    }

    Normal3d abs() const {
        return Normal3d(std::abs(x), std::abs(y), std::abs(z));
    }

    float dot(const Normal3d& n) const {
        return x * n.x + y * n.y + z * n.z;
    }

    float dot(const Vector3d& v) const {return x*v.x + y*v.y + z*v.z;}
    
    friend std::ostream& operator<<(std::ostream& os, const Normal3d& n) {
        os << "n(" << n.x << ", " << n.y << ", " << n.z << ")";
        return os;
        }
    };

inline Normal3d operator*(float s, const Normal3d& n) {
    return Normal3d(s * n.x, s * n.y, s * n.z);
}
