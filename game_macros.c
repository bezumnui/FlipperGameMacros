#include "game_macros.h"



#define HID_BT_KEYS_STORAGE_PATH EXT_PATH("apps_data/hid_ble/.bt_hid.keys")

bool game_macros_button_back(void* context) {
    GameMacrosContext* app = context;
    FURI_LOG_I("GMS", "game_macros_button_back, going to the prev scene");
    scene_manager_previous_scene(app->scene_manager);

    return true;
}

void game_macros_load_resources() {
    Storage* storage = furi_record_open(RECORD_STORAGE);

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

void game_macros_bt_enable(GameMacrosContext* app) {
    app->bt = furi_record_open(RECORD_BT);
    bt_disconnect(app->bt);
    furi_delay_ms(200);
    bt_keys_storage_set_storage_path(app->bt, HID_BT_KEYS_STORAGE_PATH);
    BleProfileHidParams hid_profile_params = {
        .device_name_prefix = "GMacros",
        .mac_xor = 003,
    };
    app->profile = bt_profile_start(app->bt, ble_profile_hid, &hid_profile_params);
    furi_check(app->profile);
    furi_hal_bt_start_advertising();
    game_macros_view_ble_profile = app->profile;
}


void game_macros_bt_free(GameMacrosContext* app) {
    bt_disconnect(app->bt);
    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);
    bt_keys_storage_set_default_path(app->bt);
    furi_check(bt_profile_restore_default(app->bt));
    furi_record_close(RECORD_BT);
}

int32_t game_macros_usb_main(void* p) {
    UNUSED(p);
    size_t heap_on_init = memmgr_get_free_heap();
    GameMacrosContext* app = malloc(sizeof(GameMacrosContext)); // 110464 - 110104 (117768 - 117616)

    Gui* gui = furi_record_open(RECORD_GUI);
    
    // memmgr_get_free_heap();
    app->menu = submenu_alloc(); // 111152

    
    // view_dispatcher
    app->view_dispatcher = view_dispatcher_alloc(); // 110832
    app->map = game_macros_script_mapping_alloc(); // 110528
    app->control = game_macros_view_control_alloc(app->map); // 61320
    
    
    
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen); //60104
    view_dispatcher_enable_queue(app->view_dispatcher); 
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app); // 59824
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, game_macros_button_back);

    view_dispatcher_add_view(app->view_dispatcher, GameMacrosMenuView, submenu_get_view(app->menu));
    view_dispatcher_add_view(app->view_dispatcher, GameMacrosControlView, game_macros_view_control_get_view(app->control)); // 59800
    // scene_manager
    app->scene_manager = scene_manager_alloc(&game_macros_main_scene_handlers, app); // 59776

    // load resources  
    game_macros_load_resources(); // 59728

    // USB / BLE setup

    app->usb_mode_prev = furi_hal_usb_get_config();
    
    #ifdef GAME_MACROS_BLE
    
        FURI_LOG_I("GMS", "Enabling BLE");
        game_macros_bt_enable(app);
    #else 
        furi_hal_usb_unlock();
        #ifndef __GM_DEBUG__
            furi_check(furi_hal_usb_set_config(&usb_hid, NULL) == true);
        #endif // 59712
    #endif
   
 

    // run
    FURI_LOG_I("GMS", "Running application"); // 58640
    scene_manager_next_scene(app->scene_manager, GameMacrosMenuScene); 
    view_dispatcher_run(app->view_dispatcher); // 1,240 bytes leak (59168) 



    view_dispatcher_remove_view(app->view_dispatcher, GameMacrosMenuView); // 57400
    view_dispatcher_remove_view(app->view_dispatcher, GameMacrosControlView); // 59736
    // stop
    
    FURI_LOG_I("GMS", "Application stop"); // 59760
    scene_manager_stop(app->scene_manager);
    
    furi_hal_usb_set_config(app->usb_mode_prev, NULL); // (57528-56776) (57376-56712)  (752 ~ 664)
    
    furi_record_close(RECORD_GUI);
    

    // free
    FURI_LOG_I("GMS", "Application free");
     #ifdef GAME_MACROS_BLE
        game_macros_bt_free(app);
    #endif
    submenu_free(app->menu);
    scene_manager_free(app->scene_manager); // 59944
    view_dispatcher_free(app->view_dispatcher); // 59992
    game_macros_view_control_free(app->control); // 60576
    game_macros_script_mapping_free(app->map); // 61792
    free(app); // 111096
    size_t leaked = heap_on_init - memmgr_get_free_heap();
    FURI_LOG_I("GMS", "exit (leaked %d)", leaked); // 111152

    return 0;
}
