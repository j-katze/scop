#pragma once

#ifndef mat4_hpp
#define mat4_hpp

#include <cstddef>

#include "vec3.hpp"

struct Mat4 {
    /* opengl expects column major ordering: element at column c, row r is v[c * 4 + r] */

    /* positions in matrix: */
    /* [ 0 4  8 12 ] */
    /* [ 1 5  9 13 ] */
    /* [ 2 6 10 14 ] */
    /* [ 3 7 11 15 ] */
    float entries[16]{};

    float&operator[] (std::size_t index);
    const float& operator[] (std::size_t index) const;

    /* returns identity matrix */
    static Mat4 identityMatrix();

    /* return pointer to the first stored number */
    const float* data() const;
};

/* multiplies two matrices */
Mat4 multiply(const Mat4& lhs, const Mat4& rhs);

/* build a scaling matrix to scale an object */
Mat4 scale(const Mat4& m, const Vec3& v);

/* builds a translation matrix to move an object */
Mat4 translate(const Mat4& m, Vec3 v);

/* builds a rotation matrix to rotate an object */
Mat4 rotate(const Mat4& m, float angleRadians, Vec3 axis);

/* builds a perspective projection matrix to make distant objects appear smaller */
Mat4 perspective(float fovRadians, float aspect, float nearPlane, float farPlane);

/* builds a view matrix for a camera */
Mat4 lookAt(Vec3 position, Vec3 target, Vec3 worldUp);

#endif

