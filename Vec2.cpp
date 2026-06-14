#include "Vec2.h"
#include <cmath>
#include <iostream>


Vec2::Vec2() = default;

Vec2::Vec2(float xin, float yin) : x(xin), y(yin) {}

bool Vec2::operator==(const Vec2 &rhs) const {
    return x == rhs.x && y == rhs.y;
}

bool Vec2::operator!=(const Vec2 &rhs) const {
    return x != rhs.x || y != rhs.y;
}

Vec2 Vec2::operator+(const Vec2 &rhs) const {
    return {x + rhs.x, y + rhs.y};
}

Vec2 Vec2::operator-(const Vec2 &rhs) const {
    return {x - rhs.x, y - rhs.y};
}

Vec2 Vec2::operator*(const float val) const {
    return {x * val, y * val};
}

Vec2 Vec2::operator/(const float val) const {
    return {x / val, y / val};
}

Vec2& Vec2::operator+=(const Vec2 &rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vec2& Vec2::operator-=(const Vec2 &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vec2& Vec2::operator/=(const float val) {
    if (val == 0) {
        std::cerr << "Cannot divide by zero.\n";
        return *this;
    }
    x /= val;
    y /= val;
    return *this;
}

Vec2& Vec2::operator*=(const float val) {
    x *= val;
    y *= val;
    return *this;
}

float Vec2::dist(const Vec2 &rhs) const {
    // float dx = rhs.x - x;
    // float dy = rhs.y - y;
    // return std::sqrt(dx * dx + dy * dy);
    return (rhs - *this).length();
}

float Vec2::distSquare(const Vec2 &rhs) const {
    // float dx = rhs.x - x;
    // float dy = rhs.y - y;
    // return std::sqrt(dx * dx + dy * dy);
    return (rhs - *this).lengthSqaure();
}

float Vec2::length() const {
    return std::sqrt(x * x + y * y);
}

float Vec2::lengthSqaure() const {
    return (x * x + y * y);
}

Vec2& Vec2::operator>>=(const Vec2 &rhs) {
    float mag = this->dist(rhs);
    x /= mag;
    y /= mag;
    return *this;
}


// int main(int argc, char * argv[])
// {
// Vec2 v1(100, 100);
// Vec2 v2(200, 200);

// v1.add(v2).scale(2).add(v2);

// std::cout << v1.x << ", " << v1.y << "\n";
// std::cout << v1.dist(v2) << "\n";
// std::cout << v1.length() << "\n";
// std::cout << (v1 == v2) << "\n";
// std::cout << (v1 == v1) << "\n";

// return 0;
// }
