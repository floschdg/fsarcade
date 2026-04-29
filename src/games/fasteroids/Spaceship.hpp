#pragma once

#include "common/math.hpp"
#include "renderer/Renderer.hpp"

struct Lazer {
    Circle circle;
    V2F32 velocity;
    void Draw();
    void Update(float dt);
};

class Spaceship {
public:
    Spaceship();
    void Reset();

    void StartBoost();
    void MoveForward(float dt);
    void RotateClockwise(float dt);
    void RotateAntiClockwise(float dt);
    Lazer ShootLazer();

    void Draw();


private:
    static constexpr float k_rotation_factor = 1.4f;


private:
    V2F32 m_pos;
    float m_angle;
    float m_speed_prop;
    Mesh m_mesh;
};

