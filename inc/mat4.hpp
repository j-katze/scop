#pragma once
#include <cmath>

struct Vec3 {
    float x, y, z;
};

struct Mat4 {
    // column major: element at row r, column c is v[c * 4 + r]
    // positions in matrix
    // [  0  1  2  3 ]
    // [  4  5  6  7 ]
    // [  8  9 10 11 ]
    // [ 12 13 14 15 ]
    float v[16]{};

    // initialize matrix to identity matrix
    // [ 1 0 0 0 ]
    // [ 0 1 0 0 ]
    // [ 0 0 1 0 ]
    // [ 0 0 0 1 ]
    // identity matrix does not do anything on its own, is used as the basis for other matrices
    static Mat4 identity() {
        Mat4 m{};
        m.v[0] = m.v[5] = m.v[10] = m.v[15] = 1.0f;
        return m;
    }

    // return pointer to the first stored number
    const float* data() const { return v; }
};

// add two vectors together
inline Vec3 operator+ (Vec3 a, Vec3 b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

// subtract vector b from vector a
inline Vec3 operator- (Vec3 a, Vec3 b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline Vec3 operator/ (Vec3 a, float scalar) {
    return {a.x / scalar, a.y / scalar, a.z / scalar};
}

inline Vec3& operator+= (Vec3& a, Vec3 b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

inline Vec3& operator-= (Vec3& a, Vec3 b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

inline Vec3& operator*= (Vec3& a, Vec3 b) {
    a.x *= b.x;
    a.y *= b.y;
    a.z *= b.z;
    return a;
}

inline Vec3& operator/= (Vec3& a, Vec3 b) {
    a.x /= b.x;
    a.y /= b.y;
    a.z /= b.z;
    return a;
}

// multiply a vector by a scalar
// a scalar is a value that has only magnitude (length) and no direction
inline Vec3 operator* (Vec3 a, float scalar) {
    return {a.x * scalar, a.y * scalar, a.z * scalar};
}

// cross product, returns a vector perpendicular to both input vectors
inline Vec3 cross (Vec3 a, Vec3 b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// dot product, multiplies two vectors and adds them up 
inline float dot (Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// scales the vector to have a length of 1
inline Vec3 normalize (Vec3 a) {
    float length = std::sqrt(dot(a, a));
    if (length == 0.0f) {
        return {0.0f, 0.0f, 0.0f};
    }
    return {a.x / length, a.y / length, a.z / length};
}

// multiplies two 4x4 matrices
inline Mat4 multiply (const Mat4& a, const Mat4& b) {
    Mat4 result{};
    for (int column = 0; column < 4; ++column) {
        for (int row = 0; row < 4; ++row) {
            for (int k = 0; k < 4; ++k) {
                result.v[column * 4 + row] += a.v[k * 4 + row] * b.v[column * 4 + k];
            }
        }
    }
    return result;
}

// builds a translation matrix to move an object
inline Mat4 translate (const Mat4& m, Vec3 p) {
    Mat4 t = Mat4::identity();
    t.v[12] = p.x;
    t.v[13] = p.y;
    t.v[14] = p.z;
    return multiply(m, t);
}

// builds a rotation matrix to rotate an object
inline Mat4 rotate (const Mat4& m, float angle, Vec3 axis) {
    axis = normalize(axis);

    const float c = std::cos(angle);
    const float s = std::sin(angle);
    const float oneMinusC = 1.0f - c;
    const float x = axis.x, y = axis.y, z = axis.z;

    Mat4 r = Mat4::identity();

    // Rodrigues' rotation formulat, stored column major
    r.v[0] = c + x * x * oneMinusC;
    r.v[1] = y * x * oneMinusC + z * s;
    r.v[2] = z * x * oneMinusC - y * s;

    r.v[4] = x * y * oneMinusC - z * s;
    r.v[5] = c + y * y * oneMinusC;
    r.v[6] = z * y * oneMinusC + x * s;

    r.v[8] = x * z * oneMinusC + y * s;
    r.v[9] = y * z * oneMinusC - x * s;
    r.v[10] = c + z * z * oneMinusC;

    return multiply(m, r);
}

// builds a perspective projection matrix to make distant objects appear smaller
inline Mat4 perspective (float fovRadians, float aspect, float nearPlane, float farPlane) {
    Mat4 m{};
    const float f = 1.0f / std::tan(fovRadians * 0.5f);

    m.v[0] = f / aspect;
    m.v[5] = f;
    m.v[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
    m.v[11] = -1.0f;
    m.v[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
    return m;
}

// builds a view matrix for a camera
inline Mat4 lookAt (Vec3 eye, Vec3 center, Vec3 up) {
    const Vec3 forward = normalize(center - eye);
    const Vec3 right = normalize(cross(forward, up));
    const Vec3 cameraUp = cross(right, forward);

    Mat4 m = Mat4::identity();

    m.v[0] = right.x;
    m.v[1] = cameraUp.x;
    m.v[2] = -forward.x;

    m.v[4] = right.y;
    m.v[5] = cameraUp.y;
    m.v[6] = -forward.y;

    m.v[8] = right.z;
    m.v[9] = cameraUp.z;
    m.v[10] = -forward.z;

    m.v[12] = -dot(right, eye);
    m.v[13] = -dot(cameraUp, eye);
    m.v[14] = dot(forward, eye);

    return m;
}

