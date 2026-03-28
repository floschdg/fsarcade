#pragma once

#include "common/math.hpp"
#include "games/fetris/Board.hpp"


class Fetromino {
public:
    enum Id : uint8_t {
        o_piece,
        s_piece,
        z_piece,
        t_piece,
        l_piece,
        j_piece,
        i_piece,
        id_count,
        id_none,
    };

    enum Rotation {
        rotate_clockwise = 1,
        rotate_counter_clockwise = 3
    };

    enum Direction {
        left = -1,
        right = 1
    };


public:
    explicit Fetromino(uint16_t* board_bitmap);
    void Reset(Id id);

    Id GetId();
    BoardPos GetPos();
    int32_t GetOri();
    void GetBitmap(uint16_t* bitmap);

    bool IsCollisionWithBoard();
    bool MaybeMoveDown();
    void MaybeMoveHorizontally(Direction direction);
    void MaybeRotate(Rotation rotation);

    void Draw();


public:
    static Id GenerateRandomId();
    static bool IsCollisionWithBoard(Id id, BoardPos pos, int32_t ori, uint16_t *board_bitmap);
    static void GetBitmap(Id id, BoardPos pos, int32_t ori, uint16_t *bitmap);
    static Color GetColor(Id id);
    static void Draw(Id id, int32_t ori, V2F32 pos, float scale);


private:
    Id m_id;
    BoardPos m_pos;
    int32_t m_ori;
    uint16_t* m_board_bitmap;
};

