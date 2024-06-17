#include "game_macros_scene_main.h"

#define FILE_NAME_LEN_MAX 254

uint32_t game_macros_exit(void* ctx) {
    GameMacrosContext* app = ctx;
    UNUSED(app);
    FURI_LOG_I("GMS", "Application exit");
    return VIEW_NONE;
}

GameMacrosScriptContext game_macros_script_contexts[100] = {0};
size_t game_macros_script_contexts_size = 0;

void game_macros_free_contexts() {
    for (size_t i = 0; i < game_macros_script_contexts_size; i++)
    {
        furi_string_free(game_macros_script_contexts[game_macros_script_contexts_size].path);
    }
    
}

bool game_macros_get_assets(
    File* file,
    const char* path,
    FuriString*** folders,
    size_t* folders_count) {
    FURI_LOG_I("GMS", "Getting assets");
    *folders = malloc(100 * sizeof(FuriString*));
    *folders_count = 0;
    FileInfo file_info;
    char name_temp[FILE_NAME_LEN_MAX];
    storage_dir_open(file, path);

    while(true) {
        if(*folders_count > 99) break;
        if(!storage_dir_read(file, &file_info, name_temp, FILE_NAME_LEN_MAX)) {
            break;
        }
        if(storage_file_is_dir(file) && name_temp[0] != '.') {
            (*folders)[*folders_count] = furi_string_alloc();
            furi_string_set((*folders)[*folders_count], name_temp);
            (*folders_count)++;
        }
    }
    storage_dir_close(file);
    return *folders_count != 0;
}

void game_macros_main_scene_submenu(void* context, uint32_t index) {
    FURI_LOG_I("GMS", "Submenu");
    GameMacrosContext* app = context;
    if(index == GameMacrosChoseControl) {
        app->view_id = GameMacrosControlView;
        view_dispatcher_switch_to_view(app->view_dispatcher, GameMacrosControlView);
    } else if(index == GameMacrosChoseExit) {
        view_dispatcher_stop(app->view_dispatcher);
    }
}

void game_macros_main_scene_enter(void* context) {
    FURI_LOG_I("GMS", "Scene enter");
    GameMacrosContext* app = context;

    // views setup
    app->menu = submenu_alloc();

    // views registrations
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);

    FuriString** folders = 0;
    size_t folders_count = 0;
    char path[] = GAME_MACROS_APP_FOLDER;
    
    if(game_macros_get_assets(file, path, &folders, &folders_count)) {
        for(size_t i = 0; i < folders_count; i++) {
            GameMacrosScriptContext* context = game_macros_script_contexts + game_macros_script_contexts_size;
            game_macros_script_contexts_size += 1;
            context->path = furi_string_alloc();
            context->ctx = app;
            FURI_LOG_I("GMS", "Adding %s", path);
            furi_string_cat_printf(context->path, "%s/%s", path, furi_string_get_cstr(folders[i]));
            submenu_add_item(
                app->menu, furi_string_get_cstr(folders[i]), i, game_macros_script_parcer, context);
            furi_string_free(folders[i]);
        }
    }
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
   
    

    submenu_add_item(app->menu, "Exit", GameMacrosChoseExit, game_macros_main_scene_submenu, app);
    app->control = game_macros_view_control_alloc();

    view_dispatcher_add_view(
        app->view_dispatcher, GameMacrosMenuView, submenu_get_view(app->menu));
    view_dispatcher_add_view(
        app->view_dispatcher,
        GameMacrosControlView,
        game_macros_view_control_get_view(app->control));
    view_set_previous_callback(submenu_get_view(app->menu), game_macros_exit);
    // USB setup
    app->usb_mode_prev = furi_hal_usb_get_config();
    furi_hal_usb_unlock();
    furi_check(furi_hal_usb_set_config(&usb_hid, NULL) == true);

    app->view_id = GameMacrosMenuView;
    view_dispatcher_switch_to_view(app->view_dispatcher, GameMacrosMenuView);
};
bool game_macros_main_scene_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return 0;
}
void game_macros_main_scene_exit(void* context) {
    GameMacrosContext* app = context;
    FURI_LOG_I("GMS", "game_macros_main_scene_exit");
    furi_hal_usb_set_config(app->usb_mode_prev, NULL);
    submenu_free(app->menu);
    game_macros_view_control_free(app->control);
    UNUSED(app);
}