#include "bn_keypad.h"
#include "scene.h"

void SceneManager::render()
{
    text_generator.generate(-120, 0, current_scene->lines[index].text[0], text_sprites[0]);
    text_generator.generate(-120, 12, current_scene->lines[index].text[1], text_sprites[1]);
    text_generator.generate(-120, 24, current_scene->lines[index].text[2], text_sprites[2]);
}

void SceneManager::update()
{
    if (keypad::a_pressed())
    {
        index++;
        if (index >= current_scene->lines_count)
        {
            index = 0;
            is_complete = true;
        }
    }
}