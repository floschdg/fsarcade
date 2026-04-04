#pragma once

#include "common/math.hpp"

#include <stdint.h>


struct Tilemap {
    Tilemap(V2F32 pos, float tile_size, int32_t tiles_x, int32_t tiles_y);

    V2F32 m_pos;
    V2F32 m_dim;
    int32_t m_tiles_x;
    int32_t m_tiles_y;
    float m_tile_size;

    float WorldX(int32_t tile_x);
    float WorldY(int32_t tile_y);
};

