#pragma once

#ifndef vec3_hpp
#define vec3_hpp

#include <cmath>
#include <stdexcept>

struct Vec3 {
    float x, y, z;

    Vec3& operator+= (const Vec3& rhs) {
       x += rhs.x;
       y += rhs.y;
       z += rhs.z;
       return *this;
    }

    Vec3& operator-= (const Vec3& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    float& operator[] (int i) {
        switch(i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("Vec3 index must be 0, 1, or 2");
        }
    }

    const float& operator[] (int i) const {
        switch(i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("Vec3 index must be 0, 1, or 2");
        }
    }

    /* get length of vector */
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    bool isZero() const {
        return x == 0.0f && y == 0.0f && z ==0.0f;
    }
};

/* add two vectors together */
inline Vec3 operator+ (Vec3 lhs, const Vec3& rhs) {
    return lhs += rhs;
}

/* subtract vector b from vector a */
inline Vec3 operator- (Vec3 lhs, const Vec3& rhs) {
    return lhs -= rhs;
}

/* multiply vector by a scalar */
/* a scalar has only magnitude (length), no direction */
inline Vec3 operator* (const Vec3& v, const float& scalar) {
    return {v.x * scalar, v.y * scalar, v.z * scalar};
}

/* devide vector by a scalar */
inline Vec3 operator/ (const Vec3& v, const float& scalar) {
    return {v.x / scalar, v.y / scalar, v.z / scalar};
}

/* cross product, returns a vector perpendicular to both input vectors */
inline Vec3 cross (const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

/* dot product, multiplies two vectors and adds them up, returns a scalar */
inline float dot (const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/* scales the vector to have a length of 1 */
inline Vec3 normalize (Vec3 v) {
    if (v.length() == 0.0f) {
        return {0.0f, 0.0f, 0.0f};
    }
    return v / v.length();
}

#endif
