#ifndef Vector4f_H
#define Vector4f_H

#include <cmath>
#include <iostream>

// Vector4f performs vector operations with 4-dimensions
// The purpose of this class is primarily for 3D graphics
// applications.
struct Vector4f {
    // Note: x,y,z,w are a convention
    // x,y,z,w could be position, but also any 4-component value.
    float x, y, z, w;

    // Default conostrutcor
    // 'why default?'
    // https://stackoverflow.com/questions/20828907/the-new-keyword-default-in-c11
    Vector4f() = default;

    // The "Real" constructor we want to use.
    // This initializes the values x,y,z
    Vector4f(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) {}

    // Index operator, allowing us to access the individual
    // x,y,z,w components of our vector.
    float& operator[](int i)
    {
        // TODO: Discuss with your partner why this works.
        //       There is no code to change here.
        return ((&x)[i]);
    }

    // Index operator, allowing us to access the individual
    // x,y,z,w components of our vector.
    const float& operator[](int i) const
    {
        // TODO: Discuss with your partner why this works.
        //       There is no code to change here.
        return ((&x)[i]);
    }

    // Multiplication Operator
    // Multiply vector by a uniform-scalar.
    Vector4f& operator*=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
        w *= s;

        return (*this);
    }

    // Division Operator
    Vector4f& operator/=(float s)
    {
        x /= s;
        y /= s;
        z /= s;
        w /= s;

        return (*this);
    }

    // Addition operator
    Vector4f& operator+=(const Vector4f& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;

        return (*this);
    }

    // Subtraction operator
    Vector4f& operator-=(const Vector4f& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;

        return (*this);
    }
};

// Compute the dot product of a Vector4f
inline float Dot(const Vector4f& a, const Vector4f& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// Multiplication of a vector by a scalar values
inline Vector4f operator*(const Vector4f& v, float s)
{
    Vector4f vec;
    vec.x = v.x * s;
    vec.y = v.y * s;
    vec.z = v.z * s;
    vec.w = v.w * s;

    return vec;
}

// Division of a vector by a scalar value.
inline Vector4f operator/(const Vector4f& v, float s)
{
    Vector4f vec;
    vec.x = v.x / s;
    vec.y = v.y / s;
    vec.z = v.z / s;
    vec.w = v.w / s;

    return vec;
}

// Negation of a vector
// Use Case: Sometimes it is handy to apply a force in an opposite direction
inline Vector4f operator-(const Vector4f& v)
{
    Vector4f vec;
    vec.x = -v.x;
    vec.y = -v.y;
    vec.z = -v.z;
    vec.w = -v.w;

    return vec;
}

// Return the magnitude of a vector
inline float Magnitude(const Vector4f& v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

// Add two vectors together
inline Vector4f operator+(const Vector4f& a, const Vector4f& b)
{
    Vector4f vec;
    vec.x = a.x + b.x;
    vec.y = a.y + b.y;
    vec.z = a.z + b.z;
    vec.w = a.w + b.w;

    return vec;
}

// Subtract two vectors
inline Vector4f operator-(const Vector4f& a, const Vector4f& b)
{
    Vector4f vec;
    vec.x = a.x - b.x;
    vec.y = a.y - b.y;
    vec.z = a.z - b.z;
    vec.w = a.w - b.w;

    return vec;
}

// Print vector to ostream
std::ostream& operator<<(std::ostream& os, const Vector4f& v)
{
    os << v.x << '\t' << v.y << '\t' << v.z << '\t' << v.w << std::endl;
    return os;
}

// Vector Projection
// Note: This is the vector projection of 'a' onto 'b'
inline Vector4f Project(const Vector4f& a, const Vector4f& b)
{
    Vector4f vec;

    float dot = Dot(a, b);
    float lb = Magnitude(b);
    vec.x = dot * b.x / lb;
    vec.y = dot * b.y / lb;
    vec.z = dot * b.z / lb;
    vec.w = dot * b.w / lb;

    return vec;
}

// Set a vectors magnitude to 1
// Note: This is NOT generating a normal vector
// TODO does this create a new vector? yes for now
inline Vector4f Normalize(const Vector4f& v)
{
    float len = Magnitude(v);
    Vector4f vec;
    vec.x = v.x / len;
    vec.y = v.y / len;
    vec.z = v.z / len;
    vec.w = v.w / len;
    return vec;
}

// a x b (read: 'a crossed b')
// Produces a new vector perpendicular to a and b.
// (So long as a and b are not parallel which returns zero vector)
// Note: For a Vector4f, we can only compute a cross porduct to
//       to vectors in 3-dimensions. Simply ignore w, and set to (0,0,0,1)
//       for this vector.
inline Vector4f CrossProduct(const Vector4f& a, const Vector4f& b)
{
    Vector4f vec;
    // from https://www.mathsisfun.com/algebra/vectors-cross-product.html
    vec.x = a.y * b.z - a.z * b.y;
    vec.y = a.z * b.x - a.x * b.z;
    vec.z = a.x * b.y - a.y * b.z;
    vec.w = 1.0f;
    return vec;
}

#endif
