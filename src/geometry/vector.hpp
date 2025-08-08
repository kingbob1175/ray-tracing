#pragma once

#include <cmath>
#include <iostream>

class Vector3d{
public:
    float x, y, z;

    Vector3d(): x(0), y(0), z(0) {}
    Vector3d(float _xx): x(_xx), y(_xx), z(_xx) {}
    Vector3d(float _xx, float _yy, float _zz): x(_xx), y(_yy), z(_zz) {}

    float getX() const { return x;}
    float getY() const { return y;}
    float getZ() const { return z;}

    Vector3d operator+(const Vector3d& v) const {return Vector3d(x+v.x, y+v.y, z+v.z);}
    Vector3d operator-(const Vector3d& v) const {return Vector3d(x-v.x, y-v.y, z-v.z);}
    Vector3d operator*(const Vector3d& v) const {return Vector3d(x*v.x, y*v.y, z*v.z);}
    Vector3d operator/(const Vector3d& v) const {return Vector3d(x/v.x, y/v.y, z/v.z);}
    Vector3d operator*(float s) const {return Vector3d(x*s, y*s, z*s);}
    Vector3d operator/(float s) const {return Vector3d(x/s, y/s, z/s);}
    Vector3d operator+=(const Vector3d& v) {x+=v.x, y+=v.y, z+=v.z; return *this;}
    Vector3d operator-=(const Vector3d& v) {x-=v.x, y-=v.y, z-=v.z; return *this;}
    Vector3d operator*=(const Vector3d& v) {x*=v.x, y*=v.y, z*=v.z; return *this;}

    friend Vector3d operator*(const float s, const Vector3d& v) {return Vector3d(v.x*s, v.y*s, v.z*s);}
    friend Vector3d operator-(const Vector3d& v) {
        return Vector3d(-v.x, -v.y, -v.z);
    }

    bool operator==(const Vector3d& v) {return x==v.x && y==v.y && z==v.z;}

    float dot(const Vector3d& v) const {return x*v.x + y*v.y + z*v.z;}
    Vector3d cross(const Vector3d& v) const {return Vector3d(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);}
    float norm() const {return std::sqrt(x*x + y*y + z*z);}
    float norm2() const {return x*x + y*y + z*z;}
    Vector3d normalize() const {return Vector3d(x/norm(), y/norm(), z/norm());}

    friend std::ostream& operator<<(std::ostream& os, const Vector3d& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

    static Vector3d Max(const Vector3d& v1, const Vector3d& v2) {
        return Vector3d(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
    }
    static Vector3d Min(const Vector3d& v1, const Vector3d& v2) {
        return Vector3d(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
    }

    float& operator[](size_t i) {
        return *(&x + i);
    }

    const float& operator[](size_t i) const {
        return *(&x + i);
    }
};

class Vector2d{
public:
    float x, y;

    Vector2d(): x(0), y(0) {}
    Vector2d(float _xx): x(_xx), y(_xx) {}
    Vector2d(float _xx, float _yy): x(_xx), y(_yy) {}

    float getX() const { return x;}
    float getY() const { return y;}

    Vector2d operator+(const Vector2d& v) const {return Vector2d(x+v.x, y+v.y);}
    Vector2d operator-(const Vector2d& v) const {return Vector2d(x-v.x, y-v.y);}
    Vector2d operator*(const Vector2d& v) const {return Vector2d(x*v.x, y*v.y);}
    Vector2d operator/(const Vector2d& v) const {return Vector2d(x/v.x, y/v.y);}
    Vector2d operator*(float s) const {return Vector2d(x*s, y*s);}
    Vector2d operator/(float s) const {return Vector2d(x/s, y/s);}
    Vector2d operator+=(const Vector2d& v) {x+=v.x, y+=v.y; return *this;}
    Vector2d operator-=(const Vector2d& v) {x-=v.x, y-=v.y; return *this;}
    friend Vector2d operator*(const float s, const Vector2d& v) {return Vector2d(v.x*s, v.y*s);}

    bool operator==(const Vector2d& v) {return x==v.x && y==v.y;}

    float dot(const Vector2d& v) const {return x*v.x + y*v.y;};
    float norm() const {return std::sqrt(x*x + y*y);}
    Vector2d normalize() const {return Vector2d(x/norm(), y/norm());}

    friend std::ostream& operator<<(std::ostream& os, const Vector2d& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }

    static Vector2d Max(const Vector2d& v1, const Vector2d& v2) {
        return Vector2d(std::max(v1.x, v2.x), std::max(v1.y, v2.y));
    }
    static Vector2d Min(const Vector2d& v1, const Vector2d& v2) {
        return Vector2d(std::min(v1.x, v2.x), std::min(v1.y, v2.y));
    }
};

inline Vector3d lerp(float t, const Vector3d& v1, const Vector3d& v2) {
    return (1-t)*v1 + t*v2;
}

inline Vector2d lerp(float t, const Vector2d& v1, const Vector2d& v2) {
    return v1*(1-t) + v2*t;
}