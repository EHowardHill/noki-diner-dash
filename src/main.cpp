/*
Cooking with Enoki
made with Butano
*/

#include "bn_core.h"
#include "gameplay.h"

#include "bn_music_items.h"

int main()
{
    bn::core::init();

    Room room;

    room.bg_music = &bn::music_items::test;

    Manager manager(room);

    while (true)
    {
        manager.update();
        bn::core::update();
    }
}