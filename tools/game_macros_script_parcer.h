#pragma once

#include <furi.h>
#include <storage/storage.h>
#include "game_macros_script_parcer.h"
#include "game_macros_hid_map.h"
#include "views/game_macros_view_control.h"
#include "scenes/game_macros_scene_main.h"
#define FILE_SIZE_MAX 100 * sizeof (char)



void game_macros_script_parcer(void* context, uint32_t index);