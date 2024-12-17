#ifndef VEC2_H
#define VEC2_H

#include <SFML/System/Vector2.hpp> // Include SFML's Vector2 definition

class Vec2
{
public:
    float x = 0;
    float y = 0;

    Vec2();
    Vec2(float xin, float yin);

    // Constructor to initialize from sf::Vector2f
    Vec2(const sf::Vector2f& v);

    // Assignment operator to allow sf::Vector2f -> Vec2
    Vec2& operator=(const sf::Vector2f& v);

    // Operator overloads
    bool operator==(const Vec2& rhs) const;
    bool operator!=(const Vec2& rhs) const;

    Vec2 operator+(const Vec2& rhs) const;
    Vec2 operator-(const Vec2& rhs) const;
    Vec2 operator/(const float val) const;
    Vec2 operator*(const float val) const;

    void operator+=(const Vec2& rhs);
    void operator-=(const Vec2& rhs);
    void operator*=(const float val);
    void operator/=(const float val);

    float dist(const Vec2& rhs) const;

    Vec2 normalize() const;
};

#endif
