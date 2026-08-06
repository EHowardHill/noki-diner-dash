#pragma once

#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_camera_ptr.h"
#include "bn_optional.h"

#include "bn_sprite_items_spr_noki.h"
#include "bn_regular_bg_items_bg_grass.h"

#define SPEED_WALK 1
#define SPEED_RUN 2
#define SPEED_WALK_ANIM 5
#define SPEED_RUN_ANIM 2

#define TILE_SIZE 32
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160

enum direction
{
    DIR_DOWN,
    DIR_RIGHT,
    DIR_LEFT,
    DIR_UP
};

struct Room
{
    int width_tiles = 32;
    int height_tiles = 32;
    const bn::regular_bg_item* bg_item = &bn::regular_bg_items::bg_grass;

    int init_tile_x = 0;
    int init_tile_y = 0;
};

struct Noki
{
    bn::sprite_ptr sprite;

    bn::optional<bn::sprite_animate_action<4>> action;

    int current_direction = DIR_DOWN;
    bool is_moving = false;
    int room_width_pixels;
    int room_height_pixels;

    Noki(bn::camera_ptr& camera, const Room& room);

    void update();
};

struct Manager
{
    const Room room;
    bn::camera_ptr camera;

    Noki noki;

    bn::optional<bn::regular_bg_ptr> bg;

    Manager(const Room room);

    void update();
};