/*
Cooking with Enoki
made with Butano
*/

#include "bn_core.h"
#include "gameplay.h"
#include "scene.h"

#include "scene01.h"

using namespace bn;

int main()
{
    core::init();

    SceneManager scene_manager;
    scene_manager.current_scene = &scene01;
    scene_manager.render();

    while (true)
    {
        core::update();
    }
}