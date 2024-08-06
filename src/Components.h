#pragma once

#include "Vec2.h"
#include <SFML/Graphics.hpp>

class CTransform
{
public:
    Vec2 pos;
    Vec2 velocity;
    float angle;
    float speed;

    CTransform(const Vec2 & p, const Vec2 & v, float a, float s)
        : pos(p), velocity(v), angle(a), speed(s) {}
};

class CShape
{
public:
    sf::CircleShape circle;
    float radius    = 0;

    CShape(float radius, int points, const sf::Color & fill, const sf::Color & outline, float thickness)
        : circle(radius, points)
    {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(radius, radius);
    }
};

class CCollision
{
public: 
    float radius = 0;
    CCollision(float r)
        : radius(r) {}
};

class CScore
{
public:
    int score = 0;
    CScore(int s)
        : score(s) {}
};

class CLifespan
{
public:
    int remaining   = 0;    // amount of lifespan remaining on the entity
    int total       = 0;    // the total inital amount of lifespan
    CLifespan(int total)
        : remaining(total), total(total) {}
};

class CInput
{
public:
    bool up     = false;
    bool left   = false;
    bool right  = false;
    bool down   = false;
    bool shoot  = false;

    CInput() {}
};