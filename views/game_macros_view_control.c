#include "game_macros_view_control.h"


FuriHalBleProfileBase* game_macros_view_ble_profile = NULL;

View* game_macros_view_control_get_view(GameMacrosViewControl* model) {
    return model->view;
}

static const NotificationSequence blink_test_sequence_hw_blink_blue = {
    &message_blink_start_10,
    &message_blink_set_color_blue,
    &message_do_not_reset,
    NULL,
};


static const NotificationSequence blink_test_sequence_hw_blink_stop = {
    &message_blink_stop,
    NULL,
};


static void game_macros_view_draw_arrow(Canvas* canvas, uint8_t x, uint8_t y, CanvasDirection dir) {
    canvas_draw_triangle(canvas, x, y, 5, 3, dir);
    if(dir == CanvasDirectionBottomToTop) {
        canvas_draw_line(canvas, x, y + 6, x, y - 1);
    } else if(dir == CanvasDirectionTopToBottom) {
        canvas_draw_line(canvas, x, y - 6, x, y + 1);
    } else if(dir == CanvasDirectionRightToLeft) {
        canvas_draw_line(canvas, x + 6, y, x - 1, y);
    } else if(dir == CanvasDirectionLeftToRight) {
        canvas_draw_line(canvas, x - 6, y, x + 1, y);
    }
}
void game_macros_view_control_free(GameMacrosViewControl* control) {
    FURI_LOG_I("GMS", "control free");
    
    with_view_model(
        control->view,
        GameMacrosViewControlModel * model,
        {
            game_macros_view_control_thread_free(model);
            furi_string_free(model->debug_string);
        },
        false
    );
    view_free(control->view);
    free(control);
    FURI_LOG_I("GMS", "control released");

}


static void game_macros_view_control_draw(Canvas* canvas, void* model_raw) {
    UNUSED(canvas);
    UNUSED(model_raw);
    GameMacrosViewControlModel * model = model_raw;
    
    // that part is using form the hid_keynote.c
    elements_multiline_text_aligned(canvas, 15, 8, AlignLeft, AlignTop, furi_string_get_cstr(model->debug_string));

    const uint8_t x_2 = 23;
    const uint8_t x_1 = 2;
    const uint8_t x_3 = 44;

    const uint8_t y_1 = 44;
    const uint8_t y_2 = 65;

    // Up
    canvas_draw_icon(canvas, x_2, y_1, &I_Button_18x18);
    if(model->up_pressed) {
        elements_slightly_rounded_box(canvas, x_2 + 3, y_1 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    game_macros_view_draw_arrow(canvas, x_2 + 9, y_1 + 6, CanvasDirectionBottomToTop);
    canvas_set_color(canvas, ColorBlack);

    // Down
    canvas_draw_icon(canvas, x_2, y_2, &I_Button_18x18);
    if(model->down_pressed) {
        elements_slightly_rounded_box(canvas, x_2 + 3, y_2 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    game_macros_view_draw_arrow(canvas, x_2 + 9, y_2 + 10, CanvasDirectionTopToBottom);
    canvas_set_color(canvas, ColorBlack);

    // Left
    canvas_draw_icon(canvas, x_1, y_2, &I_Button_18x18);
    if(model->left_pressed) {
        elements_slightly_rounded_box(canvas, x_1 + 3, y_2 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    game_macros_view_draw_arrow(canvas, x_1 + 7, y_2 + 8, CanvasDirectionRightToLeft);
    canvas_set_color(canvas, ColorBlack);

    // Right
    canvas_draw_icon(canvas, x_3, y_2, &I_Button_18x18);
    if(model->right_pressed) {
        elements_slightly_rounded_box(canvas, x_3 + 3, y_2 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    game_macros_view_draw_arrow(canvas, x_3 + 11, y_2 + 8, CanvasDirectionLeftToRight);
    canvas_set_color(canvas, ColorBlack);

    // Ok
    canvas_draw_icon(canvas, 2, 86, &I_Space_60x18);
    if(model->ok_pressed) {
        elements_slightly_rounded_box(canvas, 5, 88, 55, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 11, 90, &I_Ok_btn_9x9);
    elements_multiline_text_aligned(canvas, 26, 98, AlignLeft, AlignBottom, "Space");
    canvas_set_color(canvas, ColorBlack);

    // Back
    canvas_draw_icon(canvas, 2, 107, &I_Space_60x18);
    if(model->back_pressed) {
        elements_slightly_rounded_box(canvas, 5, 109, 55, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 11, 111, &I_Pin_back_arrow_10x8);
    elements_multiline_text_aligned(canvas, 26, 119, AlignLeft, AlignBottom, "Back");

}
void game_macros_keyboard_process(uint16_t button, bool press) {
    
    #ifdef GAME_MACROS_BLE
        FURI_LOG_I("GMS", "Keyboard process BLE");
        ble_profile_hid_kb_release(game_macros_view_ble_profile, button);
        if (press) {
            ble_profile_hid_kb_press(game_macros_view_ble_profile, button);
        } 
    #else
        FURI_LOG_I("GMS", "Keyboard process");
        furi_hal_hid_kb_release(button);
        if (press) {
            furi_hal_hid_kb_press(button);
        } 
    #endif
}



int32_t game_macros_view_control_input_play(void *context) {
    FURI_LOG_I("GMS", "Input play");
    GameMacrosControlThreadCtx* ctx = context;
    GameMacrosScriptInstuction *instruction;
    ctx->set->is_playing = true;
    for (size_t i = 0; i < ctx->count; i++)
    {
        instruction = ctx->instructions + i;
            FURI_LOG_I("GMS", "Instruction load... %d", instruction->delay_ms);

        if (instruction->delay) {
            FURI_LOG_I("GMS", "Sleep %d", instruction->delay_ms);
            if (furi_thread_flags_wait(GameMacrosThreadTerminate, FuriFlagWaitAny, instruction->delay_ms) & GameMacrosThreadTerminate) {
                return 0;
        
            } 
            continue;
        }
        UNUSED(blink_test_sequence_hw_blink_blue);
        if (instruction->key.press) {
            notification_message(ctx->notifications, &blink_test_sequence_hw_blink_blue);
        } else {
            notification_message(ctx->notifications, &blink_test_sequence_hw_blink_stop);
        }
        
        game_macros_keyboard_process(instruction->key.hid_key, instruction->key.press);
    }
    ctx->set->is_playing = false;
    return 0;
}

void game_macros_view_control_input_process(InputEvent* event, GameMacrosViewControlModel *model) {
    FURI_LOG_I("GMS", "Input process");
    GameMacrosScriptInstuctionSet *set = NULL;
    GameMacrosScriptMapping *map = model->map;
    GameMacrosScriptInstuction *instructions = NULL;
    GameMacrosControlThreadCtx* thread_ctx = NULL;

    GameMacrosViewThreads *threads = &model->threads;
    size_t count;
    furi_check(map);
    if (event->key == InputKeyBack) {
        set = &map->back;
        thread_ctx = &threads->thread_back; 
    } else if (event->key == InputKeyOk) {
        set = &map->ok;
        thread_ctx = &threads->thread_ok; 
    } else if (event->key == InputKeyUp) {
        set = &map->up;
        thread_ctx = &threads->thread_up; 
    } else if (event->key == InputKeyDown) {
        set = &map->down;
        thread_ctx = &threads->thread_down; 
    } else if (event->key == InputKeyLeft) {
        set = &map->left;
        thread_ctx = &threads->thread_left; 
    } else if (event->key == InputKeyRight) {
        set = &map->right;
        thread_ctx = &threads->thread_right;
    }
          
    furi_check(set);
    if (set->is_playing) return;

    if (event->type == InputTypeLong) {
        if (set->on_hold_size == 0) return;
        instructions = set->on_hold;
        count = set->on_hold_size;
    } else if (event->type == InputTypeShort) {
        if (set->on_short_size == 0) return;
        instructions = set->on_short;
        count = set->on_short_size;
    } else if (event->type == InputTypeRelease) {
        if (set->on_release_size == 0) return;
        instructions = set->on_release;
        count = set->on_release_size;
    } else if (event->type == InputTypePress) {
        if (set->on_press_size == 0) return;
        instructions = set->on_press;
        count = set->on_press_size;
    } else return;

    thread_ctx->count = count;
    thread_ctx->instructions = instructions;
    thread_ctx->set = set;
    FURI_LOG_I("GMS", "Thread start");
    furi_thread_start(thread_ctx->thread);
}



static bool game_macros_view_control_input(InputEvent* event, void* context) {
    
    
    GameMacrosViewControl* control = context;
    if (event->type == InputTypeLong && event->key == InputKeyBack) {
        furi_hal_hid_kb_release_all();
        return false;
    }
    with_view_model(
        control->view,
        GameMacrosViewControlModel * model,
        {   
            game_macros_view_control_input_process(event, model);
            if (event->type == InputTypePress || event->type == InputTypeRelease) {
                bool value = event->type == InputTypePress;
                if (event->key == InputKeyBack) {
                    model->back_pressed = value;
                } else if (event->key == InputKeyOk) {
                    model->ok_pressed = value;
                } else if (event->key == InputKeyUp) {
                    model->up_pressed = value;
                } else if (event->key == InputKeyDown) {
                    model->down_pressed = value;
                } else if (event->key == InputKeyLeft) {
                    model->left_pressed = value;
                } else if (event->key == InputKeyRight) {
                    model->right_pressed = value;
                }
                    
            }
        },
        true
    );

    FURI_LOG_I("GMS", "INPUT processed: true");
    return true;
}


void game_macros_view_control_thread_alloc(const char* name, size_t stack_size, GameMacrosControlThreadCtx* ctx) {
    ctx->thread = furi_thread_alloc();
    furi_thread_set_name(ctx->thread, name);
    furi_thread_set_stack_size(ctx->thread, stack_size);
    furi_thread_set_context(ctx->thread, ctx);
    furi_thread_set_callback(ctx->thread, game_macros_view_control_input_play);

}



void game_macros_view_control_thread_free(GameMacrosViewControlModel *model) {
    FURI_LOG_I("GMS", "threads free");
    furi_check(model);
    FURI_LOG_I("GMS", "threads: control checked");
    furi_record_close(RECORD_NOTIFICATION);   
    
    FuriThread* threads[] = {
        model->threads.thread_up.thread,
        model->threads.thread_down.thread,
        model->threads.thread_left.thread,
        model->threads.thread_right.thread,
        model->threads.thread_ok.thread,
        model->threads.thread_back.thread
    };
    #define THREADS_ITTERATION (size_t i = 0; i < sizeof (threads) / sizeof (FuriThread*); i++)
    
    FURI_LOG_I("GMS", "threads flag setting");
    for THREADS_ITTERATION
    {
        furi_thread_flags_set(furi_thread_get_id(threads[i]), GameMacrosThreadTerminate);
    }

    FURI_LOG_I("GMS", "threads joining");
    for THREADS_ITTERATION
    {
        furi_thread_join(threads[i]);
    }

    FURI_LOG_I("GMS", "threads releasing");
    for THREADS_ITTERATION
    {
        furi_thread_free(threads[i]);
    }
    #undef THREADS_ITTERATION

    FURI_LOG_I("GMS", "threads work done");
    
}

void game_macros_view_control_reset(GameMacrosViewControl* control) {
    with_view_model(
        control->view,
        GameMacrosViewControlModel * model,
        {
            memset(model->map, 0, sizeof (GameMacrosScriptMapping));
        },
        true
    )
}
// ViewDispatcherMessageTypeStop
GameMacrosViewControl* game_macros_view_control_alloc(GameMacrosScriptMapping* mapping) {
    FURI_LOG_I("GMS", "view control alloc");

    GameMacrosViewControl* control = malloc(sizeof (GameMacrosViewControl));
    control->view = view_alloc();
    view_allocate_model(control->view, ViewModelTypeLocking, sizeof (GameMacrosViewControlModel));
    view_set_draw_callback(control->view, game_macros_view_control_draw);
    view_set_input_callback(control->view, game_macros_view_control_input);
    view_set_context(control->view, control);
    view_set_orientation(control->view, ViewOrientationVertical);   
    with_view_model(
        control->view,
        GameMacrosViewControlModel * model,
        {
            model->map = mapping;
            model->notifications = furi_record_open(RECORD_NOTIFICATION);

            model->threads.thread_up.notifications = model->notifications;
            model->threads.thread_down.notifications = model->notifications;
            model->threads.thread_left.notifications = model->notifications;
            model->threads.thread_right.notifications = model->notifications;
            model->threads.thread_ok.notifications = model->notifications;
            model->threads.thread_back.notifications = model->notifications;

            game_macros_view_control_thread_alloc("GMA_up", 1024, &model->threads.thread_up);
            game_macros_view_control_thread_alloc("GMA_down", 1024, &model->threads.thread_down);
            game_macros_view_control_thread_alloc("GMA_left", 1024, &model->threads.thread_left);
            game_macros_view_control_thread_alloc("GMA_right", 1024, &model->threads.thread_right);
            game_macros_view_control_thread_alloc("GMA_ok", 1024, &model->threads.thread_ok);
            game_macros_view_control_thread_alloc("GMA_back", 1024, &model->threads.thread_back);

            
            model->debug_string = furi_string_alloc();
            furi_string_set(model->debug_string, "Hold to exit");
        },
        true
    );
    return control;
}







