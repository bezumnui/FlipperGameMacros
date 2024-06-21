#pragma once
#include <furi.h>
#include <furi_hal_usb.h>

#include <gui/scene_manager.h>
#include <gui/modules/popup.h>
#include "game_macros.h"
#include "tools/game_macros_script_parcer.h"
#include "views/game_macros_view_control.h"


void game_macros_main_scene_enter(void* context);
bool game_macros_main_scene_event(void* context, SceneManagerEvent event);
void game_macros_main_scene_exit(void* context);