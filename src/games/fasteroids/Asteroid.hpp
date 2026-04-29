#pragma once

#include "common/math.hpp"
#include "renderer/Renderer.hpp"


class Asteroid {
public:
    Asteroid();

    void Draw();
    void Update(float dt);

    V2F32 GetPos() {return m_pos;}

private:
    V2F32 m_pos;
    float m_speed;
    float m_angle;
    Mesh m_mesh;
};

