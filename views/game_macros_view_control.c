#include "game_macros_view_control.h"

View* game_macros_view_control_get_view(GameMacrosViewControl* model) {
    return model->view;
}
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
    if (press) {
        furi_hal_hid_kb_press(button);
    } else {
        furi_hal_hid_kb_release(button);
    }
}

static bool game_macros_view_control_input(InputEvent* event, void* context) {
    
    
    GameMacrosViewControl* control = context;
    with_view_model(
        control->view,
        GameMacrosViewControlModel * model,
        {   
            if (event->type == InputTypePress || event->type == InputTypeRelease) {
                bool value = event->type == InputTypePress;
                if (event->key == InputKeyBack) {
                    game_macros_keyboard_process(HID_KEYBOARD_E, value);

                    model->back_pressed = value;
                } else if (event->key == InputKeyOk) {
                    game_macros_keyboard_process(HID_KEYBOARD_SPACEBAR, value);
                    model->ok_pressed = value;
                } else if (event->key == InputKeyUp) {
                    game_macros_keyboard_process(HID_KEYBOARD_W, value);
                    model->up_pressed = value;
                } else if (event->key == InputKeyDown) {
                    game_macros_keyboard_process(HID_KEYBOARD_S, value);
                    model->down_pressed = value;
                } else if (event->key == InputKeyLeft) {
                    game_macros_keyboard_process(HID_KEYBOARD_A, value);

                    model->left_pressed = value;
                } else if (event->key == InputKeyRight) {
                    game_macros_keyboard_process(HID_KEYBOARD_D, value);

                    model->right_pressed = value;
                }
                    
            }
        },
        true
    );
    if (event->type == InputTypeLong && event->key == InputKeyBack) {
        furi_hal_hid_kb_release_all();
        return false;
    }
    return true;
}
GameMacrosViewControl* game_macros_view_control_alloc() {
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
            model->debug_string = furi_string_alloc();
            furi_string_set(model->debug_string, "Hold to exit");
        },
        true
    );
    return control;
}







