#include "scene01.h"

static const Line scene01_lines[] = {
    {
        .text = {
            "Hello, welcome to Cooking with Enoki!",
            "What would you like to cook today?",
            "Let's get started!"
        },
        .left_portrait = nullptr,
        .right_portrait = nullptr,
        .left_portrait_frame = 0,
        .right_portrait_frame = 0,
        .left_portrait_visible = false,
        .right_portrait_visible = false,
        .left_portrait_flipped = false,
        .right_portrait_flipped = false
    }
};

const Scene scene01 = {
    .lines = scene01_lines,
    .lines_count = 1
};
