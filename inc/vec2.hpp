#pragma once

#ifndef vec2_hpp
#define vec2_hpp

struct Vec2 {
    float x, y;

    Vec2& operator+= (const Vec2& rhs);
    Vec2& operator-= (const Vec2& rhs);
    float& operator[] (int i);
    const float& operator[] (int i) const;

    /* get length of vector */
    float length() const;
    /* return true if all members of vector are zero */
    bool isZero() const;
};

/* add two vectors together */
inline Vec2 operator+ (Vec2 lhs, const Vec2& rhs);

/* subtract vector b from vector a */
inline Vec2 operator- (Vec2 lhs, const Vec2& rhs);

/* multiply vector by a scalar */
/* a scalar has only magnitude (length), no direction */
inline Vec2 operator* (const Vec2& v, const float& scalar);

/* divide vector by a scalar */
inline Vec2 operator/ (const Vec2& v, const float& scalar);

/* cross product of two 2d vectors points along the z axis, essentially a 3d vector with only a z component,
 * since there is only one component the function returns it as a scalar */
inline float cross (const Vec2& a, const Vec2& b);

/* dot product, multiplies two vectors and adds them up, returns a scalar */
inline float dot (const Vec2& a, const Vec2& b);

/* scales the vector to have a length of 1 */
inline Vec2 normalize (Vec2 v);

#endif
