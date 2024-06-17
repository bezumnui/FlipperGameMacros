#include "game_macros.h"

bool game_macros_button_back(void* context) {
    GameMacrosContext* app = context;
    app->view_id = GameMacrosMenuView;
    view_dispatcher_switch_to_view(app->view_dispatcher, GameMacrosMenuView);

    return true;
}

void game_macros_load_resources() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    storage_file_close(file);

    UNUSED(storage);
    FURI_LOG_I("GMS", "Application free");
    if(!storage_common_exists(storage, GAME_MACROS_APP_FOLDER "/WASD_EXAMPLE")) {
        storage_common_mkdir(storage, GAME_MACROS_APP_FOLDER "/WASD_EXAMPLE");
        storage_common_copy(
            storage,
            APP_ASSETS_PATH("WASD_EXAMPLE/back.txt"),
            GAME_MACROS_APP_FOLDER "/WASD_EXAMPLE/back.txt");
        storage_common_copy(
            storage,
            APP_ASSETS_PATH("WASD_EXAMPLE/down.txt"),
            GAME_MACROS_APP_FOLDER "/WASD_EXAMPLE/down.txt");
        storage_common_copy(
            storage,
            APP_ASSETS_PATH("WASD_EXAMPLE/left.txt"),
            GAME_MACROS_APP_FOLDER "/WASD_EXAMPLE/left.txt");
        storage_common_copy(
            storage,
            APP_ASSETS_PATH("WASD_EXAMPLE/right.txt"),
            GAME_MACROS_APP_FOLDER "/WASD_EXAMPLE/right.txt");
        storage_common_copy(
            storage,
            APP_ASSETS_PATH("WASD_EXAMPLE/ok.txt"),
            GAME_MACROS_APP_FOLDER "/WASD_EXAMPLE/ok.txt");
        storage_common_copy(
            storage,
            APP_ASSETS_PATH("WASD_EXAMPLE/up.txt"),
            GAME_MACROS_APP_FOLDER "/WASD_EXAMPLE/up.txt");
    }

    furi_record_close(RECORD_STORAGE);
}

int32_t game_macros_main(void* p) {
    UNUSED(p);
    GameMacrosContext* app = malloc(sizeof(GameMacrosContext));
    Gui* gui = furi_record_open(RECORD_GUI);

    // view_dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, game_macros_button_back);

    // scene_manager
    app->scene_manager = scene_manager_alloc(&game_macros_main_scene_handlers, app);

    // USB setup
    app->usb_mode_prev = furi_hal_usb_get_config();
    furi_hal_usb_unlock();
    // furi_check(furi_hal_usb_set_config(&usb_hid, NULL) == true);

    // load resources
    game_macros_load_resources();

    // run
    FURI_LOG_I("GMS", "Running application");
    scene_manager_next_scene(app->scene_manager, GameMacrosMainScene);
    view_dispatcher_run(app->view_dispatcher);

    // free
    FURI_LOG_I("GMS", "Application free");
    furi_hal_usb_set_config(app->usb_mode_prev, NULL);
    scene_manager_stop(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);
    free(app);
    FURI_LOG_I("GMS", "exit");

    return 0;
}
