#pragma once

#include "games/Game.hpp"
#include "common/math.hpp"
#include "common/Font.hpp"

#include <random>


class Fnake : public Game {
public:
    enum Direction : int32_t {
        up,
        down,
        left,
        right,
    };


public:
    Fnake();


private:
    void Start() override;
    void ProcessEvent(SDL_Event& event) override;
    void Update(float dt) override;
    void Draw() override;

    void MaybeMoveFnake(float dt_in_seconds);
    void SpawnFood();

    void HandleGameOver();



private:
    static constexpr int32_t max_map_width = 14;
    static constexpr int32_t max_map_height = 14;
    static constexpr float tiles_per_second = 4.0f;
    static constexpr char highscore_path[] = "fnake_highscore.txt";

    Font m_font;

    static std::mt19937 s_rng;
    std::uniform_int_distribution<int32_t> m_dist;

    int32_t m_starting_map_width = 10;
    int32_t m_starting_map_height = 8;

    Direction m_direction;
    Direction m_last_advanced_direction;

    int32_t m_map_width;
    int32_t m_map_height;
    int32_t m_tail {0};
    int32_t m_head {1};
    uint64_t m_body_bitmap[max_map_height];
    V2I32 m_body_positions[max_map_width * max_map_height];
    V2I32 m_food_position;

    int32_t m_score;
    int32_t m_highscore;
};


