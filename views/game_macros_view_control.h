#pragma once

#include <furi.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>
#include <storage/storage.h>

#include <bt/bt_service/bt.h>
#include <furi_hal_bt.h>
#include <extra_profiles/hid_profile.h>
#include <notification/notification_messages.h>

#include <gui/view.h>
#include <gui/elements.h>
#include <gui/view_dispatcher.h>
#include <input/input.h>
#include "icons/hid_icons.h"
#include "tools/game_macros_hid_map.h"




typedef struct {
    bool press;
    uint16_t hid_key;
} GameMacrosScriptKey;

typedef struct {
    bool delay;
    union
    {
        GameMacrosScriptKey key;
        size_t delay_ms;
    };
} GameMacrosScriptInstuction;

typedef struct {
    GameMacrosScriptInstuction on_press[256];
    size_t on_press_size;
    GameMacrosScriptInstuction on_release[256];
    size_t on_release_size;
    GameMacrosScriptInstuction on_hold[256];
    size_t on_hold_size;
    GameMacrosScriptInstuction on_short[256];
    size_t on_short_size;
    bool is_playing;
    size_t size;
} GameMacrosScriptInstuctionSet;

typedef struct {
    GameMacrosScriptInstuctionSet up;
    GameMacrosScriptInstuctionSet down;
    GameMacrosScriptInstuctionSet left;
    GameMacrosScriptInstuctionSet right;
    GameMacrosScriptInstuctionSet ok;
    GameMacrosScriptInstuctionSet back; // the "back" do not have on_press; 
} GameMacrosScriptMapping; // 4,656 KB

typedef struct {
    bool reading_line;
    bool new_line;
    bool on_press;
    bool on_release;
    bool on_hold;
    bool on_short;
} GameMacorsReadFlags;


typedef struct
{
    View* view;
} GameMacrosViewControl;

typedef struct 
{
    GameMacrosScriptInstuctionSet* set;
    GameMacrosScriptInstuction* instructions;
    FuriThread* thread;
    size_t count;
    NotificationApp* notifications;
    
} GameMacrosControlThreadCtx;

typedef struct 
{
    GameMacrosControlThreadCtx thread_up;
    GameMacrosControlThreadCtx thread_down;
    GameMacrosControlThreadCtx thread_left;
    GameMacrosControlThreadCtx thread_right;
    GameMacrosControlThreadCtx thread_back;
    GameMacrosControlThreadCtx thread_ok;
} GameMacrosViewThreads;

typedef struct
{
    FuriString* debug_string;
    GameMacrosScriptMapping* map;
    NotificationApp* notifications;
    bool up_pressed;
    bool down_pressed;
    bool left_pressed;
    bool right_pressed;
    bool back_pressed;
    bool ok_pressed;
    GameMacrosViewThreads threads;
    
} GameMacrosViewControlModel;

typedef enum
{
    GameMacrosThreadTerminate = 1
} GameMacrosThreadFlags;


void game_macros_view_control_thread_free(GameMacrosViewControlModel *model);
void game_macros_view_control_set_map(GameMacrosViewControl* control, GameMacrosScriptMapping* map);
View* game_macros_view_control_get_view(GameMacrosViewControl* model);
GameMacrosViewControl* game_macros_view_control_alloc(GameMacrosScriptMapping* mapping);
void game_macros_view_control_free(GameMacrosViewControl* control);
void game_macros_view_control_reset(GameMacrosViewControl* control);

extern FuriHalBleProfileBase* game_macros_view_ble_profile;

