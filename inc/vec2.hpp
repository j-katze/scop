#pragma once

#ifndef vec2_hpp
#define vec2_hpp

#include <cmath>
#include <stdexcept>

struct Vec2 {
    float x, y;

    Vec2& operator+= (const Vec2& rhs) {
       x += rhs.x;
       y += rhs.y;
       return *this;
    }

    Vec2& operator-= (const Vec2& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    float& operator[] (int i) {
        switch(i) {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("Vec2 index must be 0 or 1");
        }
    }

    const float& operator[] (int i) const {
        switch(i) {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("Vec2 index must be 0 or 1");
        }
    }

    /* get length of vector */
    float length() const {
        return std::sqrt(x * x + y * y);
    }

    bool isZero() const {
        return x == 0.0f && y == 0.0f;
    }
};

/* add two vectors together */
inline Vec2 operator+ (Vec2 lhs, const Vec2& rhs) {
    return lhs += rhs;
}

/* subtract vector b from vector a */
inline Vec2 operator- (Vec2 lhs, const Vec2& rhs) {
    return lhs -= rhs;
}

/* multiply vector by a scalar */
/* a scalar has only magnitude (length), no direction */
inline Vec2 operator* (const Vec2& v, const float& scalar) {
    return {v.x * scalar, v.y * scalar};
}

/* devide vector by a scalar */
inline Vec2 operator/ (const Vec2& v, const float& scalar) {
    return {v.x / scalar, v.y / scalar};
}

/* cross product of two 2d vectors points along the z axis, essentially a 3d vector with only a z component, since there is only one component the function returns it as a scalar */
inline float cross (const Vec2& a, const Vec2& b) {
    return a.x * b.y - a.y * b.x;
}

/* dot product, multiplies two vectors and adds them up, returns a scalar */
inline float dot (const Vec2& a, const Vec2& b) {
    return a.x * b.x + a.y * b.y;
}

/* scales the vector to have a length of 1 */
inline Vec2 normalize (Vec2 v) {
    if (v.length() == 0.0f) {
        return {0.0f, 0.0f};
    }
    return v / v.length();
}

#endif
