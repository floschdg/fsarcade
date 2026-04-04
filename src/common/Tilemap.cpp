#include "common/Tilemap.hpp"

Tilemap::Tilemap(V2F32 pos, float tile_size, int32_t tiles_x, int32_t tiles_y)
    : m_pos {pos}
    , m_dim {tiles_x * tile_size, tiles_y * tile_size}
    , m_tiles_x {tiles_x}
    , m_tiles_y {tiles_y}
    , m_tile_size {tile_size}
{
}

float
Tilemap::WorldX(int32_t tile_x)
{
    float x = m_pos.x + (float)tile_x * m_tile_size;
    return x;
}

float
Tilemap::WorldY(int32_t tile_y)
{
    float y = m_pos.y + (float)tile_y * m_tile_size;
    return y;
}

