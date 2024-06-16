#pragma once

#include <furi.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>
#include <gui/view.h>
#include <gui/elements.h>
#include <gui/view_dispatcher.h>
#include <input/input.h>
#include "icons/hid_icons.h"
#include "game_macros_view_control.h"
#include "game_macros_hid_map.h"

typedef struct
{
    View* view;
} GameMacrosViewControl;


typedef struct
{
    FuriString* debug_string;
    bool up_pressed;
    bool down_pressed;
    bool left_pressed;
    bool right_pressed;
    bool back_pressed;
    bool ok_pressed;

} GameMacrosViewControlModel;

View* game_macros_view_control_get_view(GameMacrosViewControl* model);
GameMacrosViewControl* game_macros_view_control_alloc();

