#pragma once

#include <furi.h>
#include <furi_hal_usb.h>
#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>

#include "scenes/game_macros_scenes.h"
#include "views/game_macros_view_control.h"

typedef enum { GameMacrosMenuView, GameMacrosControlView } GameMacorsViews;
typedef enum {GameMacrosChoseControl, GameMacrosChoseExit} GameMacrosChoses;
typedef struct {
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    Submenu* menu;
    GameMacorsViews view_id;
    FuriHalUsbInterface* usb_mode_prev;
} GameMacrosContext;


uint32_t game_macros_exit(void* ctx);

// entry point
int32_t game_macros_main(void* p);
