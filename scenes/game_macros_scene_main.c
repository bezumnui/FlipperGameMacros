#include "game_macros_scene_main.h"


void game_macros_main_scene_submenu(void* context, uint32_t index) {
    GameMacrosContext* app = context;
    if (index == GameMacrosChoseControl) {
        view_dispatcher_switch_to_view(app->view_dispatcher, GameMacrosControlView);
    } else if (index == GameMacrosChoseExit) {
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
    }
}

void game_macros_main_scene_enter(void* context) {
    GameMacrosContext* app = context;

    // views setup
    app->menu = submenu_alloc();
    submenu_add_item(app->menu, "Control", GameMacrosChoseControl, game_macros_main_scene_submenu, app);
    submenu_add_item(app->menu, "Exit", GameMacrosChoseExit, game_macros_main_scene_submenu, app);
    GameMacrosViewControl* control = game_macros_view_control_alloc();

    // views registrations
    view_dispatcher_add_view(app->view_dispatcher, GameMacrosMenuView, submenu_get_view(app->menu));
    view_dispatcher_add_view(app->view_dispatcher, GameMacrosControlView, game_macros_view_control_get_view(control));
    view_set_previous_callback(submenu_get_view(app->menu), game_macros_exit);

    // USB setup
    app->usb_mode_prev = furi_hal_usb_get_config();
    furi_hal_usb_unlock();
    furi_check(furi_hal_usb_set_config(&usb_hid, NULL) == true);
    view_dispatcher_switch_to_view(app->view_dispatcher, GameMacrosMenuView);
};
bool game_macros_main_scene_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return 0;

}
void game_macros_main_scene_exit(void* context) {
    UNUSED(context);
}