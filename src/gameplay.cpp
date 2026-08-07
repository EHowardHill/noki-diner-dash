#include "gameplay.h"
#include "bn_keypad.h"
#include "bn_sound_items.h"

using namespace bn;

Noki::Noki(camera_ptr &_camera, const Room &room)
    : camera(_camera),
      sprite(
          sprite_items::spr_noki.create_sprite(
              room.init_tile_x * TILE_SIZE,
              room.init_tile_y * TILE_SIZE,
              0)),
      room_width_pixels(room.width_tiles * TILE_SIZE),
      room_height_pixels(room.height_tiles * TILE_SIZE)
{
    sprite.set_camera(camera);
}

void Noki::update_dust()
{
    for (DustParticle &particle : dust_particles)
    {
        if (particle.sprite)
        {
            --particle.frames_left;

            if (particle.frames_left <= 0)
            {
                // Releasing the sprite returns its OBJ resources to Butano.
                particle.sprite.reset();
            }
        }
    }
}

void Noki::emit_dust(
    int x,
    int y,
    int move_x,
    int move_y)
{
    DustParticle *free_particle = nullptr;

    for (DustParticle &particle : dust_particles)
    {
        if (!particle.sprite)
        {
            free_particle = &particle;
            break;
        }
    }

    // Do not allocate anything if the fixed particle pool is full.
    if (!free_particle)
    {
        return;
    }

    // Convert movement into direction components from -1 through 1.
    int direction_x = 0;
    int direction_y = 0;

    if (move_x < 0)
    {
        direction_x = -1;
    }
    else if (move_x > 0)
    {
        direction_x = 1;
    }

    if (move_y < 0)
    {
        direction_y = -1;
    }
    else if (move_y > 0)
    {
        direction_y = 1;
    }

    // Place the dust opposite Noki's movement direction.
    int dust_x = x - direction_x * DUST_OFFSET;
    int dust_y = y - direction_y * DUST_OFFSET;

    // bn::random's upper limit is exclusive, so adding one allows the
    // positive endpoint to be selected.
    int lateral_offset = dust_random.get_int(
        -DUST_LATERAL_RANGE,
        DUST_LATERAL_RANGE + 1);

    // A perpendicular vector to (direction_x, direction_y) is
    // (-direction_y, direction_x). This also handles diagonal movement.
    dust_x += -direction_y * lateral_offset;
    dust_y += direction_x * lateral_offset;

    free_particle->sprite =
        sprite_items::spr_dust.create_sprite(
            dust_x,
            dust_y,
            0);

    free_particle->sprite->set_camera(camera);

    // Keep the dust visually behind Noki.
    free_particle->sprite->set_z_order(
        sprite.z_order() + 1);

    // Generate an integer percentage from 20 through 100.
    int scale_percent = dust_random.get_int(20, 101);

    bn::fixed scale =
        bn::fixed(scale_percent) / 100;

    // Scaling causes Butano to assign an affine matrix to this sprite.
    free_particle->sprite->set_scale(scale);

    free_particle->frames_left = DUST_LIFETIME;
}

void Noki::update()
{
    // Existing dust must continue aging even when Noki stops moving.
    update_dust();

    const bool now_running = keypad::b_held();

    int speed = SPEED_WALK;
    int speed_anim = SPEED_WALK_ANIM;

    if (now_running)
    {
        speed = SPEED_RUN;
        speed_anim = SPEED_RUN_ANIM;
    }

    int old_x = sprite.x().integer();
    int old_y = sprite.y().integer();

    int new_x = old_x;
    int new_y = old_y;

    int new_direction = current_direction;

    if (keypad::left_held())
    {
        new_x -= speed;
        new_direction = DIR_LEFT;
    }
    else if (keypad::right_held())
    {
        new_x += speed;
        new_direction = DIR_RIGHT;
    }

    if (keypad::up_held())
    {
        new_y -= speed;
        new_direction = DIR_UP;
    }
    else if (keypad::down_held())
    {
        new_y += speed;
        new_direction = DIR_DOWN;
    }

    int min_x = -(room_width_pixels / 2) + TILE_SIZE / 2;
    int max_x = (room_width_pixels / 2) - TILE_SIZE / 2;
    int min_y = -(room_height_pixels / 2) + TILE_SIZE / 2;
    int max_y = (room_height_pixels / 2) - TILE_SIZE / 2;

    if (new_x < min_x)
    {
        new_x = min_x;
    }
    else if (new_x > max_x)
    {
        new_x = max_x;
    }

    if (new_y < min_y)
    {
        new_y = min_y;
    }
    else if (new_y > max_y)
    {
        new_y = max_y;
    }

    int move_x = new_x - old_x;
    int move_y = new_y - old_y;

    // Check actual movement after clamping. This prevents dust and walking
    // animation while Noki is pushing against the room boundary.
    bool now_moving = move_x != 0 || move_y != 0;

    sprite.set_position(new_x, new_y);

    if (now_moving)
    {
        bool animation_restarted =
            !is_moving ||
            current_direction != new_direction ||
            is_running != now_running;

        if (animation_restarted)
        {
            int base = new_direction * 3;

            action = create_sprite_animate_action_forever(
                sprite,
                speed_anim,
                sprite_items::spr_noki.tiles_item(),
                base,
                base + 1,
                base,
                base + 2);

            // The animation has just entered its first frame.
            walk_cycle_ticks = 0;
            sound_items::step_carpet.play();
        }

        action->update();

        // There are four entries in the animation sequence:
        //
        // base, base + 1, base, base + 2
        //
        // Each entry lasts speed_anim updates.
        ++walk_cycle_ticks;

        int cycle_duration = speed_anim * 4;

        if (walk_cycle_ticks >= cycle_duration)
        {
            walk_cycle_ticks = 0;

            // The animation has wrapped to its first frame again.
            sound_items::step_carpet.play();
        }

        if (dust_spawn_timer > 0)
        {
            --dust_spawn_timer;
        }

        if (dust_spawn_timer == 0)
        {
            emit_dust(new_x, new_y + 8, move_x, move_y);

            dust_spawn_timer = now_running
                                   ? DUST_RUN_INTERVAL
                                   : DUST_WALK_INTERVAL;
        }
    }
    else
    {
        dust_spawn_timer = 0;
        walk_cycle_ticks = 0;

        // is_moving contains the previous frame's movement state, so this
        // executes exactly once when Noki transitions from moving to stopped.
        if (is_moving)
        {
            action.reset();

            sprite.set_tiles(
                sprite_items::spr_noki.tiles_item(),
                current_direction * 3);

            sound_items::step_carpet.play();
        }
    }

    is_moving = now_moving;
    is_running = now_running;
    current_direction = new_direction;
}

Manager::Manager(const Room _room)
    : room(_room),
      camera(camera_ptr::create(0, 0)),
      noki(camera, room)
{
    bg = room.bg_item->create_bg(0, 0);
    bg->set_camera(camera);
    room.bg_music->play();
}

void Manager::update()
{
    noki.update();

    int cam_x = noki.sprite.x().integer();
    int cam_y = noki.sprite.y().integer();

    int room_w = room.width_tiles * TILE_SIZE;
    int room_h = room.height_tiles * TILE_SIZE;

    int min_cam_x = -(room_w / 2) + SCREEN_WIDTH / 2;
    int max_cam_x = (room_w / 2) - SCREEN_WIDTH / 2;
    int min_cam_y = -(room_h / 2) + SCREEN_HEIGHT / 2;
    int max_cam_y = (room_h / 2) - SCREEN_HEIGHT / 2;

    if (cam_x < min_cam_x)
    {
        cam_x = min_cam_x;
    }
    else if (cam_x > max_cam_x)
    {
        cam_x = max_cam_x;
    }

    if (cam_y < min_cam_y)
    {
        cam_y = min_cam_y;
    }
    else if (cam_y > max_cam_y)
    {
        cam_y = max_cam_y;
    }

    camera.set_position(cam_x, cam_y);
}