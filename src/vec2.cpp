#include "vec2.hpp"

#include <cmath>
#include <stdexcept>

Vec2& Vec2::operator+= (const Vec2& rhs) {
   x += rhs.x;
   y += rhs.y;
   return *this;
}

Vec2& Vec2::operator-= (const Vec2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

float& Vec2::operator[] (int i) {
    switch(i) {
        case 0: return x;
        case 1: return y;
        default: throw std::out_of_range("Vec2 index must be 0 or 1");
    }
}

const float& Vec2::operator[] (int i) const {
    switch(i) {
        case 0: return x;
        case 1: return y;
        default: throw std::out_of_range("Vec2 index must be 0 or 1");
    }
}

float Vec2::length() const {
    return std::sqrt(x * x + y * y);
}

bool Vec2::isZero() const {
    return x == 0.0f && y == 0.0f;
}

Vec2 operator+ (Vec2 lhs, const Vec2& rhs) {
    return lhs += rhs;
}

Vec2 operator- (Vec2 lhs, const Vec2& rhs) {
    return lhs -= rhs;
}

Vec2 operator* (const Vec2& v, const float& scalar) {
    return {v.x * scalar, v.y * scalar};
}

/* devide vector by a scalar */
Vec2 operator/ (const Vec2& v, const float& scalar) {
    return {v.x / scalar, v.y / scalar};
}

float cross (const Vec2& a, const Vec2& b) {
    return a.x * b.y - a.y * b.x;
}

float dot (const Vec2& a, const Vec2& b) {
    return a.x * b.x + a.y * b.y;
}

Vec2 normalize (Vec2 v) {
    if (v.length() == 0.0f) {
        return {0.0f, 0.0f};
    }
    return v / v.length();
}
