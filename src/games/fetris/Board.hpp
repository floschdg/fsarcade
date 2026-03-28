#pragma once

#include <cstdint>


class Fetromino;

struct BoardPos {
    int32_t x;
    int32_t y;
};


class Board {
public:
    void Reset();
    int32_t PlaceFetromino(Fetromino &fetromino);
    void Draw(int32_t level);


private:
    int32_t ClearRows(int32_t y0);


private:
    friend class Fetris;
    uint16_t m_bitmap[24];
    uint8_t m_idmap[22][10];
};

