/*

Cooking with Enoki
made with Butano

*/

#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_memory.h"
#include "bn_optional.h"

#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"

#include "bn_cameras.h"
#include "bn_camera_ptr.h"
#include "bn_sprite_animate_actions.h"

#include "bn_sprite_items_spr_noki.h"
#include "bn_regular_bg_items_bg_grass.h"

using namespace bn;

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
    const regular_bg_item &bg_item = regular_bg_items::bg_grass;

    int init_tile_x = 0;
    int init_tile_y = 0;
};

struct Noki
{
    sprite_ptr sprite;

    optional<sprite_animate_action<4>> action;

    int current_direction = DIR_DOWN;
    bool is_moving = false;
    int room_width_pixels;
    int room_height_pixels;

    Noki(camera_ptr &camera, const Room &room)
        : sprite(sprite_items::spr_noki.create_sprite(room.init_tile_x * TILE_SIZE, room.init_tile_y * TILE_SIZE, 0)),
          room_width_pixels(room.width_tiles * TILE_SIZE),
          room_height_pixels(room.height_tiles * TILE_SIZE)
    {
        sprite.set_camera(camera);
    }

    void update()
    {
        bool now_moving = false;
        int new_direction = current_direction;

        int speed = SPEED_WALK;
        int speed_anim = SPEED_WALK_ANIM;

        if (keypad::b_held())
        {
            speed = SPEED_RUN;
            speed_anim = SPEED_RUN_ANIM;
        }

        int new_x = sprite.x().integer();
        int new_y = sprite.y().integer();

        if (keypad::left_held())
        {
            new_x -= speed;
            now_moving = true;
            new_direction = DIR_LEFT;
        }
        else if (keypad::right_held())
        {
            new_x += speed;
            now_moving = true;
            new_direction = DIR_RIGHT;
        }

        if (keypad::up_held())
        {
            new_y -= speed;
            now_moving = true;
            new_direction = DIR_UP;
        }
        else if (keypad::down_held())
        {
            new_y += speed;
            now_moving = true;
            new_direction = DIR_DOWN;
        }

        int min_x = -(room_width_pixels / 2) + TILE_SIZE / 2;
        int max_x = (room_width_pixels / 2) - TILE_SIZE / 2;
        int min_y = -(room_height_pixels / 2) + TILE_SIZE / 2;
        int max_y = (room_height_pixels / 2) - TILE_SIZE / 2;

        if (new_x < min_x)
            new_x = min_x;
        if (new_x > max_x)
            new_x = max_x;
        if (new_y < min_y)
            new_y = min_y;
        if (new_y > max_y)
            new_y = max_y;

        sprite.set_position(new_x, new_y);

        if (now_moving)
        {
            if (!is_moving || current_direction != new_direction)
            {
                int base = new_direction * 3;

                action = create_sprite_animate_action_forever(
                    sprite, speed_anim, sprite_items::spr_noki.tiles_item(),
                    base, base + 1, base, base + 2);
            }

            action->update();
        }
        else if (is_moving)
        {
            action.reset();
            sprite.set_tiles(sprite_items::spr_noki.tiles_item(), current_direction * 3);
        }

        is_moving = now_moving;
        current_direction = new_direction;
    }
};

struct Manager
{
    const Room room;
    camera_ptr camera;

    Noki noki;

    optional<regular_bg_ptr> bg;

    Manager(const Room _room) : room(_room), camera(camera_ptr::create(0, 0)), noki(camera, _room)
    {
        bg = room.bg_item.create_bg(0, 0);
        bg->set_camera(camera);
        noki.sprite.set_camera(camera);
    }

    void update()
    {
        int cam_x = noki.sprite.x().integer();
        int cam_y = noki.sprite.y().integer();

        int room_w = room.width_tiles * TILE_SIZE;
        int room_h = room.height_tiles * TILE_SIZE;

        int min_cam_x = -(room_w / 2) + SCREEN_WIDTH / 2;
        int max_cam_x = (room_w / 2) - SCREEN_WIDTH / 2;
        int min_cam_y = -(room_h / 2) + SCREEN_HEIGHT / 2;
        int max_cam_y = (room_h / 2) - SCREEN_HEIGHT / 2;

        if (cam_x < min_cam_x)
            cam_x = min_cam_x;
        if (cam_x > max_cam_x)
            cam_x = max_cam_x;
        if (cam_y < min_cam_y)
            cam_y = min_cam_y;
        if (cam_y > max_cam_y)
            cam_y = max_cam_y;

        camera.set_position(cam_x, cam_y);

        noki.update();
    }
};

int main()
{
    core::init();

    const Room room01 = {
        32,
        32,
        regular_bg_items::bg_grass,
        0,
        0};

    Manager manager(room01);

    while (true)
    {
        manager.update();
        core::update();
    }
}