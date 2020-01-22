// High level design note
// Our matrix should match the behavior of the glm library.
#ifndef MATRIX4F_H
#define MATRIX4F_H

#include <cmath>
#include <iostream>

// We need to Vector4f header in order to multiply a matrix
// by a vector.
#include "Vector4f.h"

// Forward declaration
struct Matrix4f;
Matrix4f operator*(const Matrix4f& A, const Matrix4f& B);
Vector4f operator*(const Matrix4f& M, const Vector4f& v);
std::ostream& operator<<(std::ostream& os, const Matrix4f& m);

// Matrix 4f represents 4x4 matrices in Math
struct Matrix4f {
private:
    float n[4][4];  // Store each value of the matrix

public:
    Matrix4f() = default;

    // Column-major order - n[0][3] is column 0, row 3
    // Matrix constructor with 9 scalar values.
    Matrix4f(float n00, float n01, float n02, float n03, 
             float n10, float n11, float n12, float n13,
             float n20, float n21, float n22, float n23,
             float n30, float n31, float n32, float n33)
    {
        n[0][0] = n00; n[0][1] = n10; n[0][2] = n20; n[0][3] = n30;
        n[1][0] = n01; n[1][1] = n11; n[1][2] = n21; n[1][3] = n31;
        n[2][0] = n02; n[2][1] = n12; n[2][2] = n22; n[2][3] = n32;
        n[3][0] = n03; n[3][1] = n13; n[3][2] = n23; n[3][3] = n33;
    }

    // Matrix constructor from four vectors.
    // Note: 'd' will almost always be 0,0,0,1
    Matrix4f(const Vector4f& a, const Vector4f& b, const Vector4f& c,
             const Vector4f& d)
    {
        n[0][0] = a.x; n[0][1] = b.x; n[0][2] = c.x; n[0][3] = d.x;
        n[1][0] = a.y; n[1][1] = b.y; n[1][2] = c.y; n[1][3] = d.y;
        n[2][0] = a.z; n[2][1] = b.z; n[2][2] = c.z; n[2][3] = d.z;
        n[3][0] = a.w; n[3][1] = b.w; n[3][2] = c.w; n[3][3] = d.w;
    }

    // Makes the matrix an identity matrix
    void identity()
    {
        n[0][0] = 1; n[0][1] = 0; n[0][2] = 0; n[0][3] = 0;
        n[1][0] = 0; n[1][1] = 1; n[1][2] = 0; n[1][3] = 0;
        n[2][0] = 0; n[2][1] = 0; n[2][2] = 1; n[2][3] = 0;
        n[3][0] = 0; n[3][1] = 0; n[3][2] = 0; n[3][3] = 1;
    }

    // Index operator with two dimensions
    // Example: M(1,1) returns row 1 and column 1 of matrix M.
    float& operator()(int i, int j) { return (n[j][i]); }

    // Index operator with two dimensions
    // Example: M(1,1) returns row 1 and column 1 of matrix M.
    const float& operator()(int i, int j) const { return (n[j][i]); }

    // Return a single  vector from the matrix (column major).
    Vector4f& operator[](int j) { return (*reinterpret_cast<Vector4f*>(n[j])); }

    // Return a single  vector from the matrix (column major).
    const Vector4f& operator[](int j) const
    {
        return (*reinterpret_cast<const Vector4f*>(n[j]));
    }

    // from http://mathworld.wolfram.com/RotationMatrix.html

    // Make a matrix rotate about various axis
    Matrix4f MakeRotationX(float t)
    {
        return Matrix4f{1, 0, 0, 0,
            0, cosf(t), -sinf(t), 0,
            0, sinf(t), cosf(t), 0,
            0, 0, 0, 1} * (*this);
    }
    Matrix4f MakeRotationY(float t)
    {
        return Matrix4f{cosf(t), 0, -sinf(t), 0,
            0, 1, 0, 0,
            sinf(t), 0, cosf(t), 0,
            0, 0, 0, 1} * (*this);
    }
    Matrix4f MakeRotationZ(float t)
    {
        return Matrix4f{cosf(t), sinf(t), 0, 0,
                -sinf(t), cosf(t), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1} * (*this);
    }
    Matrix4f MakeScale(float sx, float sy, float sz)
    {
        (*this)[0] *= sx;
        (*this)[1] *= sy;
        (*this)[2] *= sz;

        return (*this);
    }
};

// Matrix Multiplication
Matrix4f operator*(const Matrix4f& A, const Matrix4f& B)
{
    Matrix4f mat4
    {
        // column 0
        A(0, 0) * B(0, 0) + A(0, 1) * B(1, 0) + A(0, 2) * B(2, 0) +
        A(0, 3) * B(3, 0),
        A(1, 0) * B(0, 0) + A(1, 1) * B(1, 0) + A(1, 2) * B(2, 0) + A(1, 3) * B(3, 0),
        A(2, 0) * B(0, 0) + A(2, 1) * B(1, 0) + A(2, 2) * B(2, 0) + A(2, 3) * B(3, 0),
        A(3, 0) * B(0, 0) + A(3, 1) * B(1, 0) + A(3, 2) * B(2, 0) + A(3, 3) * B(3, 0),
        // column 1
        A(0, 0) * B(0, 1) + A(0, 1) * B(1, 1) + A(0, 2) * B(2, 1) + A(0, 3) * B(3, 1),
        A(1, 0) * B(0, 1) + A(1, 1) * B(1, 1) + A(1, 2) * B(2, 1) + A(1, 3) * B(3, 1),
        A(2, 0) * B(0, 1) + A(2, 1) * B(1, 1) + A(2, 2) * B(2, 1) + A(2, 3) * B(3, 1),
        A(3, 0) * B(0, 1) + A(3, 1) * B(1, 1) + A(3, 2) * B(2, 1) + A(3, 3) * B(3, 1),
        // column 2
        A(0, 0) * B(0, 2) + A(0, 1) * B(1, 2) + A(0, 2) * B(2, 2) + A(0, 3) * B(3, 2),
        A(1, 0) * B(0, 2) + A(1, 1) * B(1, 2) + A(1, 2) * B(2, 2) + A(1, 3) * B(3, 2),
        A(2, 0) * B(0, 2) + A(2, 1) * B(1, 2) + A(2, 2) * B(2, 2) + A(2, 3) * B(3, 2),
        A(3, 0) * B(0, 2) + A(3, 1) * B(1, 2) + A(3, 2) * B(2, 2) + A(3, 3) * B(3, 2),
        // column 3
        A(0, 0) * B(0, 3) + A(0, 1) * B(1, 3) + A(0, 2) * B(2, 3) + A(0, 3) * B(3, 3),
        A(1, 0) * B(0, 3) + A(1, 1) * B(1, 3) + A(1, 2) * B(2, 3) + A(1, 3) * B(3, 3),
        A(2, 0) * B(0, 3) + A(2, 1) * B(1, 3) + A(2, 2) * B(2, 3) + A(2, 3) * B(3, 3),
        A(3, 0) * B(0, 3) + A(3, 1) * B(1, 3) + A(3, 2) * B(2, 3) + A(3, 3) * B(3, 3)
    };

    return mat4;
}

// Matrix multiply by a vector

Vector4f operator*(const Matrix4f& M, const Vector4f& v)
{
    Vector4f vec
    {
        M(0, 0) * v[0] + M(0, 1) * v[1] + M(0, 2) * v[2] + M(0, 3) * v[3],
        M(1, 0) * v[0] + M(1, 1) * v[1] + M(1, 2) * v[2] + M(1, 3) * v[3],
        M(2, 0) * v[0] + M(2, 1) * v[1] + M(2, 2) * v[2] + M(2, 3) * v[3],
        M(3, 0) * v[0] + M(3, 1) * v[1] + M(3, 2) * v[2] + M(3, 3) * v[3]
    };

    return vec;
}

std::ostream& operator<<(std::ostream& os, const Matrix4f& M)
{
    os << M(0,0) << '\t' << M(0,1) << '\t' << M(0,2) << '\t' << M(0,3) << std::endl;
    os << M(1,0) << '\t' << M(1,1) << '\t' << M(1,2) << '\t' << M(1,3) << std::endl;
    os << M(2,0) << '\t' << M(2,1) << '\t' << M(2,2) << '\t' << M(2,3) << std::endl;
    os << M(3,0) << '\t' << M(3,1) << '\t' << M(3,2) << '\t' << M(3,3) << std::endl;

    return os;
}

#endif
