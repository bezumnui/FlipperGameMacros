#pragma once

#include <furi.h>
#include <furi_hal_usb_hid.h>

typedef struct {
    char* key;
    uint16_t value;
} GameMacrosMap;

uint16_t game_macros_get_hid_pair(const char* key);
size_t game_macros_parse_number(const char* key);
size_t game_macros_string_to_int(const char* key);
extern GameMacrosMap game_macros_hid_pairs[];
