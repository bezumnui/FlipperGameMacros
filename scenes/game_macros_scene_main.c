#include "game_macros_scene_main.h"



void game_macros_main_scene_enter(void* context) {
    FURI_LOG_I("GMS", "Scene enter");
    GameMacrosContext* app = context;
   
    game_macros_script_parcer(furi_string_get_cstr(app->script_context.path), app->map);
    furi_string_free(app->script_context.path);
    app->view_id = GameMacrosControlView;
    view_dispatcher_switch_to_view(app->view_dispatcher, GameMacrosControlView);
};
bool game_macros_main_scene_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return 0;
}
void game_macros_main_scene_exit(void* context) {
    GameMacrosContext* app = context;
    FURI_LOG_I("GMS", "game_macros_main_scene_exit");
    // free
    FURI_LOG_I("GMS", "game_macros_view_control_free");
    game_macros_view_control_reset(app->control);
    
}