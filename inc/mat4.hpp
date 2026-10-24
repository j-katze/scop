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
        /* [ 1 0 0 0 ] */
        /* [ 0 1 0 0 ] */
        /* [ 0 0 1 0 ] */
        /* [ 0 0 0 1 ] */
        /* identity matrix does not do anything on its own, is used as the basis for other matrices */
    static Mat4 identityMatrix();

    /* return pointer to the first stored number */
    const float* data() const;
};

/* multiplies two matrices */
Mat4 multiply(const Mat4& lhs, const Mat4& rhs);

/* build a scaling matrix to scale an object */
    /* formula for scaleing matrix */
    /* where S is scaling vector */

    /* [ Sx  0  0 0] */
    /* [  0 Sy  0 0] */
    /* [  0  0 Sz 0] */
    /* [  0  0  0 1] */
inline Mat4 scale(const Mat4& m, const Vec3& v);

/* builds a translation matrix to move an object */
    /* formula for translation matrix */
    /* where T is translation vector */

    /* [ 1 0 0 Tx ] */
    /* [ 0 1 0 Ty ] */
    /* [ 0 0 1 Tz ] */
    /* [ 0 0 0  1 ] */
inline Mat4 translate(const Mat4& m, Vec3 v);

/* builds a rotation matrix to rotate an object
   angle needs to be provided in radians */
Mat4 rotate(const Mat4& m, float angle, Vec3 axis);

// builds a perspective projection matrix to make distant objects appear smaller
    /* formula for perspective projection matrix */
    /* where f is focal length, Z is depth (Znear is near plane, Zfar is far plane) */

    /* [ f 0                          0                         0 ] */
    /* [ 0 f                          0                         0 ] */
    /* [ 0 0  (Zfar+Znear)/(Znear-Zfar) 2*Zfar*Znear/(Znear-Zfar) ] */
    /* [ 0 0                         -1                         0 ] */

    /* formula for deriving focal length from FOV */

    /* 1/2*tan(FOV/2) */
    /* dividing focal length by aspect ratio in m[0] avoids image stretching when window is not a perfect square */
Mat4 perspective(float fovRadians, float aspect, float nearPlane, float farPlane);

// builds a view matrix for a camera
    /* formula for lookat matrix */
    /* where R is right vector, U is up vector, D is direction (forward) vector, P is camera position vector */

    /* [ Rx Ry Rz 0 ]   [ 1 0 0 -Px ] */
    /* [ Ux Uy Uz 0 ]   [ 0 1 0 -Py ] */
    /* [ Dx Dy Dz 0 ] * [ 0 0 1 -Pz ] */
    /* [  0  0  0 1 ]   [ 0 0 0   1 ] */
Mat4 lookAt(Vec3 position, Vec3 target, Vec3 worldUp);

#endif

