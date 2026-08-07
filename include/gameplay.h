#pragma once

#include "bn_fixed.h"
#include "bn_random.h"
#include "bn_array.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_camera_ptr.h"
#include "bn_music_item.h"

#include "bn_sprite_items_spr_noki.h"
#include "bn_sprite_items_spr_dust.h"

#include "bn_regular_bg_items_bg_grass.h"

#define SPEED_WALK 1
#define SPEED_RUN 2
#define SPEED_WALK_ANIM 5
#define SPEED_RUN_ANIM 2

#define TILE_SIZE 32
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160

using namespace bn;

enum direction
{
    DIR_DOWN,
    DIR_RIGHT,
    DIR_LEFT,
    DIR_UP
};

enum holding
{
    HOLDING_NONE,
    HOLDING_CUSTOMERS,
    HOLDING_TICKET,
    HOLDING_FOOD,
    HOLDING_BUS
};

struct Room
{
    int width_tiles = 32;
    int height_tiles = 32;
    const regular_bg_item *bg_item =
        &regular_bg_items::bg_grass;

    int init_tile_x = 0;
    int init_tile_y = 0;

    const music_item* bg_music = nullptr;
};

struct DustParticle
{
    optional<sprite_ptr> sprite;
    int frames_left = 0;
};

struct Noki
{
    static constexpr int DUST_PARTICLE_COUNT = 8;
    static constexpr int DUST_LIFETIME = 18;
    static constexpr int DUST_OFFSET = 10;
    static constexpr int DUST_LATERAL_RANGE = 6;
    static constexpr int DUST_WALK_INTERVAL = 10;
    static constexpr int DUST_RUN_INTERVAL = 5;

    camera_ptr camera;
    sprite_ptr sprite;

    optional<sprite_animate_action<4>> action;
    array<DustParticle, DUST_PARTICLE_COUNT> dust_particles;

    random dust_random;

    int room_width_pixels;
    int room_height_pixels;

    int current_direction = DIR_DOWN;
    int dust_spawn_timer = 0;
    int walk_cycle_ticks = 0;

    bool is_moving = false;
    bool is_running = false;

    int holding_state = HOLDING_NONE;

    Noki(camera_ptr &camera, const Room &room);

    void update();

private:
    void update_dust();
    void emit_dust(int x, int y, int move_x, int move_y);
};

struct Manager
{
    const Room room;
    camera_ptr camera;

    Noki noki;

    optional<regular_bg_ptr> bg;

    Manager(const Room room);

    void update();
};