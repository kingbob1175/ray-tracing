#pragma once

#include "geometry/geometry.hpp"
#include "basicmath/math.hpp"
#include "ray/ray.hpp"
#include <cmath>
#include <cstring>
#include <cassert>
#include <iostream>

struct Matrix4x4 {
    float m[4][4];

    Matrix4x4() {
        memset(m, 0, sizeof(m));
        for (int i = 0; i < 4; ++i)
            m[i][i] = 1.0f;
    }

    Matrix4x4(float mat[4][4]) {
        memcpy(m, mat, sizeof(float) * 16);
    }

    Matrix4x4(float m00, float m01, float m02, float m03,
              float m10, float m11, float m12, float m13,
              float m20, float m21, float m22, float m23,
              float m30, float m31, float m32, float m33) {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }

    // transpose
    Matrix4x4 transpose() const {
        Matrix4x4 r;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                r.m[i][j] = m[j][i];
        return r;
    }

    // multiply two matrices
    Matrix4x4 operator*(const Matrix4x4& mat) const {
        Matrix4x4 r;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                r.m[i][j] = 0;
                for (int k = 0; k < 4; ++k)
                    r.m[i][j] += m[i][k] * mat.m[k][j];
            }
        return r;
    }

    // return true if two matrices are equal
    bool operator==(const Matrix4x4& mat) const {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (m[i][j] != mat.m[i][j])
                    return false;
        return true;
    }

    // print out the matrix
    friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& mat) {
        for (int i = 0; i < 4; ++i) {
            os << "| ";
            for (int j = 0; j < 4; ++j)
                os << mat.m[i][j] << " ";
            os << "|\n";
        }
        return os;
    }

    // scale matrix
    static Matrix4x4 Scale(float sx, float sy, float sz) {
        return Matrix4x4(
            sx, 0,  0,  0,
            0,  sy, 0,  0,
            0,  0,  sz, 0,
            0,  0,  0,  1
        );
    }

    // translate matrix
    static Matrix4x4 Translate(float tx, float ty, float tz) {
        return Matrix4x4(
            1, 0, 0, tx,
            0, 1, 0, ty,
            0, 0, 1, tz,
            0, 0, 0, 1
        );
    }

    static Matrix4x4 rotatearoundaxis(float theta, const Vector3d& axis){
        Vector3d a = axis.normalize();
        float x = a.x, y = a.y, z = a.z; 
        float cosT = std::cos(theta);
        float sinT = std::sin(theta);
        float oneMinusCos = 1.0f - cosT;

        return Matrix4x4(
        cosT + x * x * oneMinusCos,
        x * y * oneMinusCos - z * sinT,
        x * z * oneMinusCos + y * sinT,
        0,

        y * x * oneMinusCos + z * sinT,
        cosT + y * y * oneMinusCos,
        y * z * oneMinusCos - x * sinT,
        0,

        z * x * oneMinusCos - y * sinT,
        z * y * oneMinusCos + x * sinT,
        cosT + z * z * oneMinusCos,
        0,

        0, 0, 0, 1
        );
    }

    // identity matrix
    static Matrix4x4 Identity() {
        return Matrix4x4();
    }

    // inverse matrix
    Matrix4x4 inverse() const {
        const float* a = &m[0][0];
        float inv[16];

        inv[0] = a[5]  * a[10] * a[15] - 
                a[5]  * a[11] * a[14] - 
                a[9]  * a[6]  * a[15] + 
                a[9]  * a[7]  * a[14] +
                a[13] * a[6]  * a[11] - 
                a[13] * a[7]  * a[10];

        inv[4] = -a[4]  * a[10] * a[15] + 
                a[4]  * a[11] * a[14] + 
                a[8]  * a[6]  * a[15] - 
                a[8]  * a[7]  * a[14] - 
                a[12] * a[6]  * a[11] + 
                a[12] * a[7]  * a[10];

        inv[8] = a[4]  * a[9] * a[15] - 
                a[4]  * a[11] * a[13] - 
                a[8]  * a[5] * a[15] + 
                a[8]  * a[7] * a[13] + 
                a[12] * a[5] * a[11] - 
                a[12] * a[7] * a[9];

        inv[12] = -a[4]  * a[9] * a[14] + 
                a[4]  * a[10] * a[13] +
                a[8]  * a[5] * a[14] - 
                a[8]  * a[6] * a[13] - 
                a[12] * a[5] * a[10] + 
                a[12] * a[6] * a[9];

        inv[1] = -a[1]  * a[10] * a[15] + 
                a[1]  * a[11] * a[14] + 
                a[9]  * a[2] * a[15] - 
                a[9]  * a[3] * a[14] - 
                a[13] * a[2] * a[11] + 
                a[13] * a[3] * a[10];

        inv[5] = a[0]  * a[10] * a[15] - 
                a[0]  * a[11] * a[14] - 
                a[8]  * a[2] * a[15] + 
                a[8]  * a[3] * a[14] + 
                a[12] * a[2] * a[11] - 
                a[12] * a[3] * a[10];

        inv[9] = -a[0]  * a[9] * a[15] + 
                a[0]  * a[11] * a[13] + 
                a[8]  * a[1] * a[15] - 
                a[8]  * a[3] * a[13] - 
                a[12] * a[1] * a[11] + 
                a[12] * a[3] * a[9];

        inv[13] = a[0]  * a[9] * a[14] - 
                a[0]  * a[10] * a[13] - 
                a[8]  * a[1] * a[14] + 
                a[8]  * a[2] * a[13] + 
                a[12] * a[1] * a[10] - 
                a[12] * a[2] * a[9];

        inv[2] = a[1]  * a[6] * a[15] - 
                a[1]  * a[7] * a[14] - 
                a[5]  * a[2] * a[15] + 
                a[5]  * a[3] * a[14] + 
                a[13] * a[2] * a[7] - 
                a[13] * a[3] * a[6];

        inv[6] = -a[0]  * a[6] * a[15] + 
                a[0]  * a[7] * a[14] + 
                a[4]  * a[2] * a[15] - 
                a[4]  * a[3] * a[14] - 
                a[12] * a[2] * a[7] + 
                a[12] * a[3] * a[6];

        inv[10] = a[0]  * a[5] * a[15] - 
                a[0]  * a[7] * a[13] - 
                a[4]  * a[1] * a[15] + 
                a[4]  * a[3] * a[13] + 
                a[12] * a[1] * a[7] - 
                a[12] * a[3] * a[5];

        inv[14] = -a[0]  * a[5] * a[14] + 
                a[0]  * a[6] * a[13] + 
                a[4]  * a[1] * a[14] - 
                a[4]  * a[2] * a[13] - 
                a[12] * a[1] * a[6] + 
                a[12] * a[2] * a[5];

        inv[3] = -a[1] * a[6] * a[11] + 
                a[1] * a[7] * a[10] + 
                a[5] * a[2] * a[11] - 
                a[5] * a[3] * a[10] - 
                a[9] * a[2] * a[7] + 
                a[9] * a[3] * a[6];

        inv[7] = a[0] * a[6] * a[11] - 
                a[0] * a[7] * a[10] - 
                a[4] * a[2] * a[11] + 
                a[4] * a[3] * a[10] + 
                a[8] * a[2] * a[7] - 
                a[8] * a[3] * a[6];

        inv[11] = -a[0] * a[5] * a[11] + 
                a[0] * a[7] * a[9] + 
                a[4] * a[1] * a[11] - 
                a[4] * a[3] * a[9] - 
                a[8] * a[1] * a[7] + 
                a[8] * a[3] * a[5];

        inv[15] = a[0] * a[5] * a[10] - 
                a[0] * a[6] * a[9] - 
                a[4] * a[1] * a[10] + 
                a[4] * a[2] * a[9] + 
                a[8] * a[1] * a[6] - 
                a[8] * a[2] * a[5];

        float det = a[0] * inv[0] + a[1] * inv[4] + a[2] * inv[8] + a[3] * inv[12];

        if (det == 0) {
            std::cerr << "Matrix is singular and cannot be inverted.\n";
            return Matrix4x4(); 
        }

        det = 1.0f / det;

        float invOut[4][4];
        for (int i = 0; i < 16; ++i)
            invOut[i / 4][i % 4] = inv[i] * det;

        return Matrix4x4(invOut);
    }

};


class Transformation {
public:
    Matrix4x4 m;
    Matrix4x4 m_inv;

    Transformation() {}

    Transformation(float mat[4][4]){
        m = Matrix4x4(mat);
        m_inv = m.transpose(); // inverse function is not finished yet
    }

    Transformation(const Matrix4x4 &m, const Matrix4x4 &mInv): m(m), m_inv(mInv) {}

    Transformation transpose() {
        return Transformation(m.transpose(), m_inv.transpose());
    }

    Transformation inverse() {
        return Transformation(m_inv, m);
    }

    Transformation operator*(const Transformation &t) const {
        return Transformation(m*t.m, t.m_inv*m_inv);
    }

    Vector3d operator()(const Vector3d &v) const {
        return Vector3d(m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z,
            m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z,
            m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z);
    }

    Point3d operator()(const Point3d &p) const {
        return Point3d(m.m[0][0]*p.x + m.m[0][1]*p.y + m.m[0][2]*p.z + m.m[0][3],
            m.m[1][0]*p.x + m.m[1][1]*p.y + m.m[1][2]*p.z + m.m[1][3],
            m.m[2][0]*p.x + m.m[2][1]*p.y + m.m[2][2]*p.z + m.m[2][3]);
    }

    Normal3d operator()(const Normal3d &n) const {
        return Normal3d(m_inv.m[0][0]*n.x + m_inv.m[0][1]*n.y + m_inv.m[0][2]*n.z,
            m_inv.m[1][0]*n.x + m_inv.m[1][1]*n.y + m_inv.m[1][2]*n.z,
            m_inv.m[2][0]*n.x + m_inv.m[2][1]*n.y + m_inv.m[2][2]*n.z);
    }

    Ray operator()(const Ray &r) const{
        return Ray((*this)(r.origin), (*this)(r.dir));
    }

    Bound3 operator()(const Bound3 &b) const {
        Bound3 ret = Bound3((*this)(Point3d(b.pMin.x, b.pMin.y, b.pMin.z)));
        ret = Union(ret, (*this)(Point3d(b.pMax.x, b.pMin.y, b.pMin.z)));
        ret = Union(ret, (*this)(Point3d(b.pMin.x, b.pMax.y, b.pMin.z)));
        ret = Union(ret, (*this)(Point3d(b.pMin.x, b.pMin.y, b.pMax.z)));
        ret = Union(ret, (*this)(Point3d(b.pMax.x, b.pMax.y, b.pMin.z)));
        ret = Union(ret, (*this)(Point3d(b.pMax.x, b.pMin.y, b.pMax.z)));
        ret = Union(ret, (*this)(Point3d(b.pMin.x, b.pMax.y, b.pMax.z)));
        ret = Union(ret, (*this)(Point3d(b.pMax.x, b.pMax.y, b.pMax.z)));
        return ret;
    }

    bool HasScale() const {
        float la2 = (*this)(Vector3d(1, 0, 0)).norm2();
        float lb2 = (*this)(Vector3d(0, 1, 0)).norm2();
        float lc2 = (*this)(Vector3d(0, 0, 1)).norm2();
        #define NOT_ONE(x) ((x) < .999f || (x) > 1.001f)
        return (NOT_ONE(la2) || NOT_ONE(lb2) || NOT_ONE(lc2));
        #undef NOT_ONE
    }
};

inline Transformation Translate(const Vector3d &delta) {
    Matrix4x4 m = Matrix4x4::Translate(delta.x, delta.y, delta.z);
    Matrix4x4 minv = Matrix4x4::Translate(-delta.x, -delta.y, -delta.z);
    return Transformation(m, minv);
}

inline Transformation Scale(float sx, float sy, float sz) {
    Matrix4x4 m = Matrix4x4::Scale(sx, sy, sz);
    Matrix4x4 m_inv = Matrix4x4::Scale(1.0f/sx, 1.0f/sy, 1.0f/sz);
    return Transformation(m, m_inv);
}

inline Transformation rotate(float theta, const Vector3d& axis){
    Matrix4x4 m = Matrix4x4::rotatearoundaxis(theta, axis);
    Matrix4x4 m_inv = Matrix4x4::rotatearoundaxis(-theta, axis);
    return Transformation(m, m_inv);
}

// camera to world
inline Transformation LookAt(const Point3d& pos, const Point3d& target, const Vector3d& up) {
    Vector3d dir = (target - pos).normalize();
    Vector3d left = up.cross(dir).normalize();
    Vector3d newup = dir.cross(left).normalize();
    Matrix4x4 m = Matrix4x4(
        left.x, newup.x, -dir.x, pos.x,
        left.y, newup.y, -dir.y, pos.y,
        left.z, newup.z, -dir.z, pos.z,
        0, 0, 0, 1
    );
    Matrix4x4 m_inv = m.inverse();
    return Transformation(m, m_inv);
}