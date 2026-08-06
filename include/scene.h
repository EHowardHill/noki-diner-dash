#pragma once

#include "bn_sprite_ptr.h"
#include "bn_string_view.h"
#include "bn_sprite_item.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "common_variable_8x8_sprite_font.h"

using namespace bn;

struct Line
{
    bn::string_view text[3];
    const bn::sprite_item *left_portrait;
    const bn::sprite_item *right_portrait;
    int left_portrait_frame;
    int right_portrait_frame;
    bool left_portrait_visible;
    bool right_portrait_visible;
    bool left_portrait_flipped;
    bool right_portrait_flipped;
};

struct Scene
{
    const Line *lines;
    int lines_count;
    bn::sprite_text_generator text_generator{common::variable_8x8_sprite_font};
};

struct SceneManager
{
    const Scene *current_scene;
    int index;
    bool is_complete;

    bn::sprite_text_generator text_generator{common::variable_8x8_sprite_font};
    bn::vector<bn::sprite_ptr, 32> text_sprites[3];

    void render();
    void update();
};