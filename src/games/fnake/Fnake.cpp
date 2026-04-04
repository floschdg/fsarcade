#include "games/fnake/Fnake.hpp"
#include "common/shapes.hpp"
#include "games/Game.hpp"
#include "renderer/Renderer.hpp"
#include "common/MemoryManager.hpp"
#include "common/math.hpp"
#include "common/Tilemap.hpp"

#include <imgui.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>


Fnake::Fnake()
    : m_font {k_dejavu_sans_mono_filepath, 22}
    , m_rng {std::random_device{}()}
    , m_tilemap {{k_tilemap_x, k_tilemap_y}, k_tile_size, k_tiles_x, k_tiles_y}
{
    static_assert(k_tiles_x <= sizeof(m_body_bitmap[0])*8);
    static_assert(k_tiles_y <= sizeof(m_body_bitmap[0])*8);

    int32_t highscore = 0;
    std::ifstream highscore_file_in {"fnake_highscore.txt"};
    if (highscore_file_in) {
        highscore_file_in >> highscore;
        highscore_file_in.close();
    }
    m_highscore = highscore;
}

void
Fnake::Start()
{
    m_game_status = game_resume;
    m_tlast_milliseconds = SDL_GetTicks();

    m_curr_direction = right;
    m_next_direction = right;
    m_tile_progress = 0.0f;

    memset(m_body_bitmap, 0, sizeof(m_body_bitmap));

    int32_t head_x = k_tiles_x / 2;
    int32_t head_y = k_tiles_y / 2;
    m_body_parts.clear();
    m_body_parts.emplace_front(V2I32{head_x-1, head_y}, right, none);
    m_body_parts.emplace_front(V2I32{head_x, head_y}, none, left);

    SpawnFood();

    m_score = 0;
}

void
Fnake::Update(float dt)
{
    MoveBody(dt);
}

void
Fnake::MoveBody(float dt)
{
    float tile_progress = m_tile_progress + k_tiles_per_second * dt;
    while (tile_progress >= 1.0f) {
        V2I32 head_pos = m_body_parts.front().tile_pos;
        V2I32 tail_pos = m_body_parts.back().tile_pos;


        // find next head_pos
        if (m_next_direction == up) {
            head_pos.y += 1;
        }
        else if (m_next_direction == down) {
            head_pos.y -= 1;
        }
        else if (m_next_direction == right) {
            head_pos.x += 1;
        }
        else if (m_next_direction == left) {
            head_pos.x -= 1;
        }
        if ((head_pos.x < 0 || head_pos.x >= k_tiles_x) ||
            (head_pos.y < 0 || head_pos.y >= k_tiles_y))
        {
            HandleGameOver();
            return;
        }


        // check collision
        uint64_t head_bit = 1 << head_pos.x;
        uint64_t body_bits = m_body_bitmap[head_pos.y];
        if (head_pos.y == tail_pos.y) {
            body_bits &= (uint32_t)~(1 << tail_pos.x);
        }
        if (head_bit & body_bits) {
            HandleGameOver();
            return;
        }


        // advance head
        m_body_bitmap[head_pos.y] |= (1 << head_pos.x);
        m_body_parts.front().next_direction_to_head = m_next_direction;
        m_body_parts.emplace_front(head_pos, none, (Direction)-m_next_direction);


        // eat food or advance tail
        if (m_body_parts.front().tile_pos == m_food_tile_pos) {
            // eat food
            m_score += 1;
            SpawnFood();
        }
        else {
            // advance tail
            m_body_bitmap[tail_pos.y] &= (uint32_t)~(1 << tail_pos.x);
            m_body_parts.pop_back();
            m_body_parts.back().next_direction_to_tail = none;
        }


        m_curr_direction = m_next_direction;
        tile_progress -= 1.0f;
    }

    m_tile_progress = tile_progress;
}

void
Fnake::HandleGameOver()
{
    m_game_status = game_over;
    if (m_score > m_highscore) {
        m_highscore = m_score;

        std::ofstream highscore_file_out {k_highscore_path};
        if (highscore_file_out) {
            highscore_file_out << m_highscore << std::endl;
            highscore_file_out.close();
        }
        else {
            SDL_LogInfo(0, "Fnake: cannot open %s for writing", k_highscore_path);
        }
    }
}

void
Fnake::ProcessEvent(SDL_Event& event)
{
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: {
        if (event.key.key == SDLK_UP) {
            if (m_curr_direction == right ||
                m_curr_direction == left)
            {
                m_next_direction = up;
            }
        }
        else if (event.key.key == SDLK_DOWN) {
            if (m_curr_direction == right ||
                m_curr_direction == left)
            {
                m_next_direction = down;
            }
        }
        else if (event.key.key == SDLK_RIGHT) {
            if (m_curr_direction == up ||
                m_curr_direction == down)
            {
                m_next_direction = right;
            }
        }
        else if (event.key.key == SDLK_LEFT) {
            if (m_curr_direction == up ||
                m_curr_direction == down)
            {
                m_next_direction = left;
            }
        }
    }

    default:;
    }
}

void
Fnake::SpawnFood()
{
    int32_t bit0_counts[k_tiles_y];
    int32_t bit0_count_total = 0;

    // count bits
    for (int32_t y = 0; y < k_tiles_y; y++) {
        int32_t bit1_count = 0;

        uint64_t bitmap_row = m_body_bitmap[y];
        while (bitmap_row != 0) {
            bitmap_row = bitmap_row & (bitmap_row - 1);
            bit1_count += 1;
        }

        int32_t bit0_count = k_tiles_x - bit1_count;
        bit0_counts[y] = bit0_count;
        bit0_count_total += bit0_count;
    }

    if (bit0_count_total == 0) {
        return;
    }

    m_dist.param(std::uniform_int_distribution<int32_t>::param_type(0, bit0_count_total - 1));
    int32_t bit0_index = m_dist(m_rng);
    int32_t bit0_x = 0;
    int32_t bit0_y = 0;

    // find y
    for (int32_t y = 0; y < k_tiles_y; y++) {
        if (bit0_index < bit0_counts[y]) {
            bit0_y = y;
            break;
        }
        bit0_index -= bit0_counts[y];
    }

    // find x
    uint64_t bitmap_row_not = ~m_body_bitmap[bit0_y];
    for (int32_t x = 0; x < k_tiles_x; x++) {
        if (bitmap_row_not & 1) {
            if (bit0_index == 0) {
                bit0_x = x;
                break;
            }
            bit0_index--;
        }
        bitmap_row_not >>= 1;
    }

    m_food_tile_pos = {bit0_x, bit0_y};
}

void
Fnake::Draw()
{
    Color tilemap_color = {0.0f, 0.0f, 0.0f, 1.0f};
    Rectangle tilemap_rect = {
        m_tilemap.m_pos.x,
        m_tilemap.m_pos.y,
        m_tilemap.m_pos.x + m_tilemap.m_dim.x,
        m_tilemap.m_pos.y + m_tilemap.m_dim.y,
    };
    g_renderer.PushRectangle(tilemap_rect, tilemap_color, k_z_tilemap);


    DrawFood();
    DrawBody();
}

static Rectangle
rect_towards_direction(Rectangle origin, Fnake::Direction direction, float x, float y, float tile_size)
{
    Rectangle rect;
    if (direction == Fnake::up) {
        rect.x0 = origin.x0;
        rect.x1 = origin.x1;
        rect.y0 = origin.y1;
        rect.y1 = y + tile_size;
    }
    else if (direction == Fnake::down) {
        rect.x0 = origin.x0;
        rect.x1 = origin.x1;
        rect.y0 = y;
        rect.y1 = origin.y0;
    }
    else if (direction == Fnake::right) {
        rect.x0 = origin.x1;
        rect.x1 = x + tile_size;
        rect.y0 = origin.y0;
        rect.y1 = origin.y1;
    }
    else if (direction == Fnake::left) {
        rect.x0 = x;
        rect.x1 = origin.x0;
        rect.y0 = origin.y0;
        rect.y1 = origin.y1;
    }
    return rect;
}

void
Fnake::DrawBody()
{
    Color body_color = {0.0f, 0.5f, 0.0f, 1.0f};
    float bodypart_size = 0.8f * k_tile_size;
    float bodypart_offset = (k_tile_size - bodypart_size) / 2;


    // draw head
    auto curr = m_body_parts.begin();
    {
        float x = m_tilemap.WorldX(curr->tile_pos.x);
        float y = m_tilemap.WorldY(curr->tile_pos.y);
        Rectangle rect = {
            x + bodypart_offset,
            y + bodypart_offset,
            rect.x0 + bodypart_size,
            rect.y0 + bodypart_size
        };

        if (curr->next_direction_to_tail == up) {
            rect.y0 += bodypart_size * (1-m_tile_progress);
            rect.y1 = y + k_tile_size;
        }
        else if (curr->next_direction_to_tail == down) {
            rect.y0 = y;
            rect.y1 -= bodypart_size * (1-m_tile_progress);
        }
        else if (curr->next_direction_to_tail == right) {
            rect.x0 += bodypart_size * (1-m_tile_progress);
            rect.x1 = x + k_tile_size;
        }
        else if (curr->next_direction_to_tail == left) {
            rect.x0 = x;
            rect.x1 -= bodypart_size * (1-m_tile_progress);
        }
        g_renderer.PushRectangle(rect, body_color, k_z_body);
    }


    // draw remaining body
    curr++;
    auto next = curr + 1;
    while (curr != m_body_parts.end()) {
        float x = m_tilemap.WorldX(curr->tile_pos.x);
        float y = m_tilemap.WorldY(curr->tile_pos.y);


        Rectangle rect = {
            x + bodypart_offset,
            y + bodypart_offset,
            rect.x0 + bodypart_size,
            rect.y0 + bodypart_size
        };
        g_renderer.PushRectangle(rect, body_color, k_z_body);


        Direction next_direction_to_head = curr->next_direction_to_head;
        Rectangle rect_to_head = rect_towards_direction(rect, next_direction_to_head, x, y, k_tile_size);
        g_renderer.PushRectangle(rect_to_head, body_color, k_z_body);


        Direction next_direction_to_tail = curr->next_direction_to_tail;
        Rectangle rect_to_tail = rect_towards_direction(rect, next_direction_to_tail, x, y, k_tile_size);
        g_renderer.PushRectangle(rect_to_tail, body_color, k_z_body);


        curr = next;
        next = next + 1;
    }
}

void
Fnake::DrawFood()
{
    float bodypart_size = 0.8f * k_tile_size;
    float bodypart_offset = (k_tile_size - bodypart_size) / 2;

    Color food_color = {0.5f, 0.0f, 0.0f, 1.0f};
    float food_x = m_tilemap.WorldX(m_food_tile_pos.x);
    float food_y = m_tilemap.WorldY(m_food_tile_pos.y);

    Rectangle rect = {
        food_x + bodypart_offset,
        food_y + bodypart_offset,
        rect.x0 + bodypart_size,
        rect.y0 + bodypart_size
    };
    g_renderer.PushRectangle(rect, food_color, k_z_food);
}

void
Fnake::DrawScores()
{
    String32Id score_label = MemoryManager::EmplaceString32_Frame(U"Score");
    String32Id score_value = MemoryManager::EmplaceString32_Frame(int32_to_u32string(m_score));

    String32Id highscore_label = MemoryManager::EmplaceString32_Frame(U"Highscore");
    String32Id highscore_value = MemoryManager::EmplaceString32_Frame(int32_to_u32string(m_highscore));


    V2F32 score_pos = {1.3f, 2.5f};
    V2F32 highscore_pos = {2.3f, 2.5f};
    Color anyscore_color {0.9f, 0.9f, 0.9f, 1.0f};

    g_renderer.PushString32(highscore_label, m_font, highscore_pos, anyscore_color, k_z_text);
    highscore_pos.y -= 0.1f;
    g_renderer.PushString32(highscore_value, m_font, highscore_pos, anyscore_color, k_z_text);

    g_renderer.PushString32(score_label, m_font, score_pos, anyscore_color, k_z_text);
    score_pos.y -= 0.1f;
    g_renderer.PushString32(score_value, m_font, score_pos, anyscore_color, k_z_text);
}

