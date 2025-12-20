// Math.h
#pragma once

#include <cmath>

struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;
    
    Vector2() = default;
    Vector2(float _x, float _y) : x(_x), y(_y) {}
    
    float distance(const Vector2& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
    
    float distanceSquared(const Vector2& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        return dx * dx + dy * dy;
    }
};

struct Vector2i {
    int x = 0;
    int y = 0;
    
    Vector2i() = default;
    Vector2i(int _x, int _y) : x(_x), y(_y) {}
};
