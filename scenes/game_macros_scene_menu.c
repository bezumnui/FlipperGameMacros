#include "game_macros_scene_menu.h"
#include <furi.h>
#include <furi_hal_usb.h>

#include <gui/scene_manager.h>
#include <gui/modules/popup.h>

#define FILE_NAME_LEN_MAX 254

uint32_t game_macros_menu_exit(void* ctx) {
    GameMacrosContext* app = ctx;
    UNUSED(app);
    FURI_LOG_I("GMS", "Application exit");
    return VIEW_NONE;
}

void game_macros_menu_exit_menu(void* ctx, uint32_t index) {
    GameMacrosContext* app = ctx;
    UNUSED(index);
    FURI_LOG_I("GMS", "Application exit from menu");
    view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_NONE);
}
GameMacrosScriptContext game_macros_script_contexts[100] = {0};
size_t game_macros_script_contexts_size = 0;

void game_macros_script_contexts_free() {
    FURI_LOG_I(
        "GMS", "game_macros_script_contexts_free, size is %d", game_macros_script_contexts_size);
    for(size_t i = 0; i < game_macros_script_contexts_size; ++i) {
        FURI_LOG_I("GMS", "game_macros_script_contexts_free loop. I: %d", i);
        furi_string_free(game_macros_script_contexts[i].path);
    }


}

bool game_macros_get_assets(
    File* file,
    const char* path,
    FuriString*** folders,
    size_t* folders_count) {

    FURI_LOG_I("GMS", "Getting assets");
    *folders = malloc(100 * sizeof(void*));
    *folders_count = 0;
    FileInfo file_info;
    char name_temp[FILE_NAME_LEN_MAX];
    storage_dir_open(file, path);
    game_macros_script_contexts_size = 0;

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

void game_macros_menu_scene_menu_callback(void* ctx, uint32_t index) {
    UNUSED(index);
    GameMacrosScriptContext* context = ctx;
    GameMacrosContext* gma_ctx = context->ctx;
    gma_ctx->script_context.path = furi_string_alloc_move(context->path);
    scene_manager_next_scene(gma_ctx->scene_manager, GameMacrosMainScene);
}

void game_macros_menu_scene_enter(void* context) {
    GameMacrosContext* app = context; // 59616
    UNUSED(context);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage); 
    FuriString** folders = 0;
    size_t folders_count = 0;
    char path[] = GAME_MACROS_APP_FOLDER;  

    if(game_macros_get_assets(file, path, &folders, &folders_count)) {
        for(size_t i = 0; i < folders_count; i++) { 
            GameMacrosScriptContext* context =
                &game_macros_script_contexts[game_macros_script_contexts_size++]; 
            context->ctx = app;
            context->path =
                furi_string_alloc_printf("%s/%s", path, furi_string_get_cstr(folders[i]));
            FURI_LOG_I("GMS", "Adding %s", furi_string_get_cstr(context->path)); 
            submenu_add_item(
                app->menu,
                furi_string_get_cstr(folders[i]),
                i,
                game_macros_menu_scene_menu_callback,
                context);
            furi_string_free(folders[i]); 
        }
    }
    free(folders);

    FURI_LOG_I("GMS", "Closing the file"); // 59584
    storage_file_free(file);
    FURI_LOG_I("GMS", "Closing the storage"); // 59616
    furi_record_close(RECORD_STORAGE);

    FURI_LOG_I("GMS", "Assets loaded");
    submenu_add_item(app->menu, "Exit", GameMacrosChoseExit, game_macros_menu_exit_menu, app);
    
    view_set_previous_callback(submenu_get_view(app->menu), game_macros_menu_exit); // 59168
    FURI_LOG_I("GMS", "switch to view");

    // view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_NONE);
    view_dispatcher_switch_to_view(app->view_dispatcher, GameMacrosMenuView);
}

bool game_macros_menu_scene_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return 0;
}

void game_macros_menu_scene_exit(void* context) {
    // if (true) return;
    FURI_LOG_I("GMS", "Menu scene exit"); 

    GameMacrosContext* app = context;
    UNUSED(app);
    game_macros_script_contexts_free(); 
    submenu_reset(app->menu); 
}