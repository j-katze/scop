#include "mat4.hpp"

#include <cmath>

float& Mat4::operator[](std::size_t index) {
    return entries[index];
}

const float& Mat4::operator[](std::size_t index) const {
    return entries[index];
}

Mat4 Mat4::identityMatrix() {
    /* [ 1 0 0 0 ] */
    /* [ 0 1 0 0 ] */
    /* [ 0 0 1 0 ] */
    /* [ 0 0 0 1 ] */
    /* identity matrix is used as the basis for other matrices, does not do anything on its own */

    Mat4 m{};

    m[0] = m[5] = m[10] = m[15] = 1.0f;

    return m;
}

const float* Mat4::data() const {
    return entries;
}

Mat4 multiply(const Mat4& lhs, const Mat4& rhs) {
    Mat4 result{};

    for (int column = 0; column < 4; ++column) {
        for (int row = 0; row < 4; ++row) {
            for (int i = 0; i < 4; ++i) {
                result[column * 4 + row] += lhs[i * 4 + row] * rhs[column * 4 + i];
            }
        }
    }

    return result;
}

Mat4 scale(const Mat4& m, const Vec3& v) {
    /* formula for scaling matrix */
    /* where S is scaling vector */

    /* [ Sx  0  0 0] */
    /* [  0 Sy  0 0] */
    /* [  0  0 Sz 0] */
    /* [  0  0  0 1] */

    Mat4 s = Mat4::identityMatrix();

    s[0] = v.x;
    s[5] = v.y;
    s[10] = v.z;

    return multiply(m, s);
}

Mat4 translate(const Mat4& m, Vec3 v) {
    /* formula for translation matrix */
    /* where T is translation vector */

    /* [ 1 0 0 Tx ] */
    /* [ 0 1 0 Ty ] */
    /* [ 0 0 1 Tz ] */
    /* [ 0 0 0  1 ] */

    Mat4 t = Mat4::identityMatrix();

    t[12] = v.x;
    t[13] = v.y;
    t[14] = v.z;

    return multiply(m, t);
}

Mat4 rotate(const Mat4& m, float angleRadians, Vec3 axis) {
    /* formula for rotation matrix for arbitrary angle
     * where R is rotation vector (Rx is x axis, Ry is y axis, Rz is z axis),
     * θ is angle vector (needs to be unit vector) */

    /* [     cosθ+Rx^2*(1−cosθ)   Rx*Ry*(1−cosθ)−Rz*sinθ   Rx*Rz*(1−cosθ)+Ry*sinθ   0 ] */
    /* [ Ry*Rx*(1−cosθ)+Rz*sinθ       cosθ+Ry^2*(1−cosθ)   Ry*Rz*(1−cosθ)−Rx*sinθ   0 ] */
    /* [ Rz*Rx*(1−cosθ)−Ry*sinθ   Rz*Ry*(1−cosθ)+Rx*sinθ       cosθ+Rz^2*(1−cosθ)   0 ] */
    /* [                      0                        0                        0   1 ] */

    const float c = std::cos(angleRadians);
    const float s = std::sin(angleRadians);
    const float oneMinusC = 1.0f - c;
    const float x = axis.x, y = axis.y, z = axis.z;

    Mat4 r = Mat4::identityMatrix();

    axis = normalize(axis);

    r[0] = c + x * x * oneMinusC;
    r[1] = y * x * oneMinusC + z * s;
    r[2] = z * x * oneMinusC - y * s;

    r[4] = x * y * oneMinusC - z * s;
    r[5] = c + y * y * oneMinusC;
    r[6] = z * y * oneMinusC + x * s;

    r[8] = x * z * oneMinusC + y * s;
    r[9] = y * z * oneMinusC - x * s;
    r[10] = c + z * z * oneMinusC;

    return multiply(m, r);
}

Mat4 perspective(float fovRadians, float aspect, float nearPlane, float farPlane) {
    /* formula for perspective projection matrix */
    /* where f is focal length, Z is depth (Znear is near plane, Zfar is far plane) */

    /* [ f 0                          0                         0 ] */
    /* [ 0 f                          0                         0 ] */
    /* [ 0 0  (Zfar+Znear)/(Znear-Zfar) 2*Zfar*Znear/(Znear-Zfar) ] */
    /* [ 0 0                         -1                         0 ] */

    /* formula for deriving focal length from FOV */

    /* 1/2*tan(FOV/2) */
    /* dividing focal length by aspect ratio in m[0] avoids image stretching when window is not a perfect square */

    Mat4 m{};
    const float f = 1.0f / std::tan(fovRadians / 2);

    m[0] = f / aspect;
    m[5] = f;
    m[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
    m[11] = -1.0f;
    m[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);

    return m;
}

Mat4 lookAt(Vec3 position, Vec3 target, Vec3 worldUp) {
    /* formula for lookat matrix */
    /* where R is right vector, U is up vector, D is direction (forward) vector, P is camera position vector */

    /* [ Rx Ry Rz 0 ]   [ 1 0 0 -Px ] */
    /* [ Ux Uy Uz 0 ]   [ 0 1 0 -Py ] */
    /* [ Dx Dy Dz 0 ] * [ 0 0 1 -Pz ] */
    /* [  0  0  0 1 ]   [ 0 0 0   1 ] */

    const Vec3 forward = normalize(target - position);
    const Vec3 right = normalize(cross(forward, worldUp));
    const Vec3 up = cross(right, forward);

    Mat4 translation = Mat4::identityMatrix();
    Mat4 rotation = Mat4::identityMatrix();

    rotation[0] = right.x;
    rotation[1] = up.x;
    rotation[2] = -forward.x;

    rotation[4] = right.y;
    rotation[5] = up.y;
    rotation[6] = -forward.y;

    rotation[8] = right.z;
    rotation[9] = up.z;
    rotation[10] = -forward.z;

    translation[12] = -position.x;
    translation[13] = -position.y;
    translation[14] = -position.z;

    return multiply(rotation, translation);
}

