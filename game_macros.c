#include "game_macros.h"


uint32_t game_macros_exit(void* ctx) {
    UNUSED(ctx);
    return VIEW_NONE;
}
bool game_macros_test() {
    return true;
}



bool game_macros_button_back(void* context) {
    GameMacrosContext* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, GameMacrosMenuView);
    return true;
}



int32_t game_macros_main(void *p) {
    UNUSED(p);
    GameMacrosContext* app = malloc(sizeof (GameMacrosContext));
    Gui* gui = furi_record_open(RECORD_GUI);

    // view_dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, game_macros_button_back);

    // scene_manager
    app->scene_manager = scene_manager_alloc(&game_macros_main_scene_handlers, app);

    
    // run
    scene_manager_next_scene(app->scene_manager, GameMacrosMainScene);
    view_dispatcher_run(app->view_dispatcher);

    // free
    // GameMacrosContext* app = context;
    furi_hal_usb_set_config(app->usb_mode_prev, NULL);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);
    free(app);



    return 0;
}
