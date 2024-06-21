#pragma once

#include <furi.h>
#include <storage/storage.h>
#include "game_macros_script_parcer.h"
#include "game_macros_hid_map.h"
#include "views/game_macros_view_control.h"
#define FILE_SIZE_MAX 100 * sizeof (char)

typedef struct
{
    FuriString* path;
    void* ctx;
} GameMacrosScriptContext;

extern GameMacrosScriptContext game_macros_script_contexts[100];
extern size_t game_macros_script_contexts_size;


void game_macros_script_parcer(const char* path, GameMacrosScriptMapping *map);
void game_macros_script_mapping_free(GameMacrosScriptMapping* mapping);
GameMacrosScriptMapping* game_macros_script_mapping_alloc();