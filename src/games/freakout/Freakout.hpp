#pragma once

#include "games/Game.hpp"
#include "common/shapes.hpp"


class Freakout : public Game {
    struct Paddle {
        float x;
        float dx;
    };

    struct Ball {
        Circle circle;
        float dx;
        float dy;
    };

    static constexpr float MAP_WIDTH = 4.0f;
    static constexpr float MAP_HEIGHT = 3.0f;

    static constexpr uint32_t BRICK_ROWS = 8;
    static constexpr uint32_t BRICK_COLS = 14;

    static constexpr float BALL_SPEED = 2.0f;

    static constexpr float PADDLE_WIDTH = 0.6f;
    static constexpr float PADDLE_HEIGHT = 0.1f;
    static constexpr float PADDLE_SPEED = 1.0f;


private:
    void Start() override;
    void ProcessEvent(SDL_Event& event) override;
    void Update(float dt) override;
    void Draw() override;

private:
    void MovePaddle(float dt);
    void MoveBall(float dt);

private:
    Rectangle m_bricks[BRICK_ROWS][BRICK_COLS];
    Paddle m_paddle;
    Ball m_ball;
};

