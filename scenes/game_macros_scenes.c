#include "game_macros_scenes.h"
#include "game_macros_scene_main.h"

AppSceneOnEnterCallback game_macros_main_on_enter_handlers[] = {
    game_macros_main_scene_enter,
};

AppSceneOnEventCallback game_macros_main_on_event_handlers[] = {
    game_macros_main_scene_event,
};
AppSceneOnExitCallback game_macros_main_on_exit_handlers[] = {
    game_macros_main_scene_exit,
};


SceneManagerHandlers game_macros_main_scene_handlers  = {
    .on_enter_handlers = game_macros_main_on_enter_handlers,
    .on_event_handlers = game_macros_main_on_event_handlers,
    .on_exit_handlers = game_macros_main_on_exit_handlers,
    .scene_num = GameMacrosCountScene
};