#include "Vec2.h"
#include <SFML/System/Vector2.hpp>
#include <cmath>

// Default constructor
Vec2::Vec2()
{
}

// Constructor with two floats
Vec2::Vec2(float xin, float yin)
    : x(xin), y(yin)
{
}

// Constructor to initialize from sf::Vector2f
Vec2::Vec2(const sf::Vector2f& v)
    : x(v.x), y(v.y)
{
}

// Assignment operator for sf::Vector2f
Vec2& Vec2::operator=(const sf::Vector2f& v)
{
    x = v.x;
    y = v.y;
    return *this;
}

// Remaining methods unchanged
Vec2 Vec2::operator+(const Vec2& rhs) const
{
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator/(const float val) const
{
    return Vec2(x / val, y / val);
}

Vec2 Vec2::operator*(const float val) const
{
    return Vec2(x * val, y * val);
}

bool Vec2::operator==(const Vec2& rhs) const
{
    return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator!=(const Vec2& rhs) const
{
    return (x != rhs.x || y != rhs.y);
}

void Vec2::operator+=(const Vec2& rhs)
{
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator-=(const Vec2& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}

void Vec2::operator*=(const float val)
{
    x *= val;
    y *= val;
}

void Vec2::operator/=(const float val)
{
    x /= val;
    y /= val;
}

float Vec2::dist(const Vec2& rhs) const
{
    return std::sqrt((x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y));
}

Vec2 Vec2::normalize() const
{
    float magnitude = std::sqrt(x * x + y * y);
    if (magnitude > 0)
    {
        return Vec2(x / magnitude, y / magnitude);
    }
    return Vec2(0, 0);
}
