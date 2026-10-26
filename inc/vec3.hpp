#pragma once

#ifndef vec3_hpp
#define vec3_hpp

struct Vec3 {
    float x, y, z;

    Vec3& operator+= (const Vec3& rhs);
    Vec3& operator-= (const Vec3& rhs);
    float& operator[] (int i);
    const float& operator[] (int i) const;

    /* returns length of vector */
    float length() const;
    /* returns true if all members of vector are zero */
    bool isZero() const;
};

/* adds two vectors together */
Vec3 operator+ (Vec3 lhs, const Vec3& rhs);

/* subtracts vector rhs from vector lhs */
Vec3 operator- (Vec3 lhs, const Vec3& rhs);

/* multiplies a vector by a scalar */
Vec3 operator* (const Vec3& v, const float& scalar);

/* devide vector by a scalar */
Vec3 operator/ (const Vec3& v, const float& scalar);

/* cross product, returns a vector perpendicular to both input vectors */
Vec3 cross (const Vec3& a, const Vec3& b);

/* dot product, multiplies two vectors and adds them up, returns a scalar */
float dot (const Vec3& a, const Vec3& b);

/* scales the vector to have a length of 1 */
Vec3 normalize (Vec3 v);

#endif
