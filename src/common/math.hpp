#pragma once

#include <cstddef>
#include <cstdint>
#include <common/shapes.hpp>
#include <algorithm>


struct V2ST {
    size_t x;
    size_t y;

    bool operator==(V2ST &b);
    bool operator==(const V2ST& other) const {
        return x == other.x && y == other.y;
    }
};

struct V2F32 {
    float x;
    float y;

    V2F32 operator/(float scalar);
    V2F32 operator*(float scalar);
    V2F32 operator+(V2F32 other);
};

struct V3F32 {
    float x;
    float y;
    float z;

    V3F32 operator/(float scalar);
    V3F32 operator*(float scalar);
};

struct V4F32 {
    float x;
    float y;
    float z;
    float w;

    V4F32 operator/(float scalar);
    V4F32 operator*(float scalar);
};

struct V2I32 {
    int32_t x;
    int32_t y;

    bool operator==(V2I32 other);
};

struct Color {
    float r;
    float g;
    float b;
    float a;
};


inline bool
Intersect_AABB_Circle(Rectangle rect, Circle circle)
{
    float xmin = std::min(rect.x0, rect.x1);
    float xmax = std::max(rect.x0, rect.x1);
    float ymin = std::min(rect.y0, rect.y1);
    float ymax = std::max(rect.y0, rect.y1);

    float closest_x = std::max(xmin, std::min(circle.x, xmax));
    float closest_y = std::max(ymin, std::min(circle.y, ymax));

    float dx = closest_x - circle.x;
    float dy = closest_y - circle.y; 
    float d_sq = dx*dx + dy*dy;

    float r_sq = circle.r * circle.r;

    bool is_intersect = d_sq <= r_sq;
    return is_intersect;
}

