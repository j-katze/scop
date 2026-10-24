#include "vec3.hpp"

#include <cmath>
#include <stdexcept>

Vec3& Vec3::operator+= (const Vec3& rhs) {
   x += rhs.x;
   y += rhs.y;
   z += rhs.z;
   return *this;
}

Vec3& Vec3::operator-= (const Vec3& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

float& Vec3::operator[] (int i) {
    switch(i) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: throw std::out_of_range("Vec3 index must be 0, 1, or 2");
    }
}

const float& Vec3::operator[] (int i) const {
    switch(i) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: throw std::out_of_range("Vec3 index must be 0, 1, or 2");
    }
}

float Vec3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

bool Vec3::isZero() const {
    return x == 0.0f && y == 0.0f && z ==0.0f;
}

Vec3 operator+ (Vec3 lhs, const Vec3& rhs) {
    return lhs += rhs;
}

Vec3 operator- (Vec3 lhs, const Vec3& rhs) {
    return lhs -= rhs;
}

Vec3 operator* (const Vec3& v, const float& scalar) {
    return {v.x * scalar, v.y * scalar, v.z * scalar};
}

Vec3 operator/ (const Vec3& v, const float& scalar) {
    return {v.x / scalar, v.y / scalar, v.z / scalar};
}

Vec3 cross (const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float dot (const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 normalize (Vec3 v) {
    if (v.length() == 0.0f) {
        return {0.0f, 0.0f, 0.0f};
    }
    return v / v.length();
}
