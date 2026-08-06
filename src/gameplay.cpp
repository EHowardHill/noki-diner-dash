#include "gameplay.h"
#include "bn_keypad.h"

using namespace bn;

Noki::Noki(camera_ptr& camera, const Room& room)
    : sprite(sprite_items::spr_noki.create_sprite(room.init_tile_x * TILE_SIZE, room.init_tile_y * TILE_SIZE, 0)),
      room_width_pixels(room.width_tiles * TILE_SIZE),
      room_height_pixels(room.height_tiles * TILE_SIZE)
{
    sprite.set_camera(camera);
}

void Noki::update()
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

    if (new_x < min_x) new_x = min_x;
    if (new_x > max_x) new_x = max_x;
    if (new_y < min_y) new_y = min_y;
    if (new_y > max_y) new_y = max_y;

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

Manager::Manager(const Room _room)
    : room(_room),
      camera(camera_ptr::create(0, 0)),
      noki(camera, room)
{
    bg = room.bg_item->create_bg(0, 0);
    bg->set_camera(camera);
}

void Manager::update()
{
    int cam_x = noki.sprite.x().integer();
    int cam_y = noki.sprite.y().integer();

    int room_w = room.width_tiles * TILE_SIZE;
    int room_h = room.height_tiles * TILE_SIZE;

    int min_cam_x = -(room_w / 2) + SCREEN_WIDTH / 2;
    int max_cam_x = (room_w / 2) - SCREEN_WIDTH / 2;
    int min_cam_y = -(room_h / 2) + SCREEN_HEIGHT / 2;
    int max_cam_y = (room_h / 2) - SCREEN_HEIGHT / 2;

    if (cam_x < min_cam_x) cam_x = min_cam_x;
    if (cam_x > max_cam_x) cam_x = max_cam_x;
    if (cam_y < min_cam_y) cam_y = min_cam_y;
    if (cam_y > max_cam_y) cam_y = max_cam_y;

    camera.set_position(cam_x, cam_y);

    noki.update();
}