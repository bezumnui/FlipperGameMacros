#include "game_macros_script_parcer.h"

void game_macros_read_flags_increase_state(GameMacorsReadFlags* flags) {
    if(flags->on_press) {
        flags->on_press = false;
        flags->on_release = true;
    } else if(flags->on_release) {
        flags->on_release = false;
        flags->on_hold = true;
    } else if(flags->on_hold) {
        flags->on_hold = false;
        flags->on_short = true;
    }
}

GameMacrosScriptInstuction* games_macros_read_flags_get_instruction(
    GameMacorsReadFlags* flags,
    GameMacrosScriptInstuctionSet* set) {
    if(flags->on_press) {
        return set->on_press;
    } else if(flags->on_release) {
        return set->on_release;
    } else if(flags->on_hold) {
        return set->on_hold;
    } else if(flags->on_short) {
        return set->on_short;
    } else
        return 0;
}

void games_macros_read_flags_set_instruction_counter(
    GameMacorsReadFlags* flags,
    GameMacrosScriptInstuctionSet* set,
    size_t value) {
    if(flags->on_press) {
        set->on_press_size = value;
    } else if(flags->on_release) {
        set->on_release_size = value;
    } else if(flags->on_hold) {
        set->on_hold_size = value;
    } else if(flags->on_short) {
        set->on_short_size = value;
    }
}

void game_macros_script_read_file(File* file, const char* path, GameMacrosScriptInstuctionSet* set) {
    if(!storage_file_open(file, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E("GMS", "Could not open %s", path);
        storage_file_close(file);
    }
    char buffer[FILE_SIZE_MAX] = {};

    size_t read = storage_file_read(file, buffer, FILE_SIZE_MAX);
    if(read == 0) return;

    GameMacorsReadFlags flags = {0};
    flags.reading_line = true;
    flags.new_line = true;
    flags.on_press = true;

    char current_char;

    uint16_t counter = 0;

    while(read != 0) {
        for(size_t i = 0; i < read; i++) {
            current_char = buffer[i];
            if(current_char == ';' || current_char == ' ') {
                flags.reading_line = false;

                continue;
            }
            if(current_char == '\0' || current_char == '\n') {
                flags.reading_line = true;
                flags.new_line = true;
                FURI_LOG_I("GMS", "new line");
                continue;
            }

            if(!flags.reading_line) {
                continue;
            }

            GameMacrosScriptInstuction* instruction =
                games_macros_read_flags_get_instruction(&flags, set);
            if(flags.new_line) {
                size_t number = game_macros_parse_number(buffer + i);
                if(number != 0) {
                    FURI_LOG_I("GMS", "Parced number: %d", number);
                    instruction[counter].delay = true;
                    instruction[counter].delay_ms = number;

                    games_macros_read_flags_set_instruction_counter(&flags, set, ++counter);
                    flags.reading_line = false;

                    continue;
                } else if(current_char == '=') {
                    game_macros_read_flags_increase_state(&flags);
                    counter = 0;
                    flags.reading_line = false;
                    flags.new_line = false;
                    continue;
                } else if(current_char == '!') {
                    instruction[counter].delay = false;
                    instruction[counter].key.press = true;
                    flags.new_line = false;
                    continue;
                } else if(current_char == '^') {
                    instruction[counter].delay = false;
                    instruction[counter].key.press = false;
                    flags.new_line = false;
                    continue;
                } else {
                    furi_crash("INVALID LINE");
                }
                flags.new_line = false;
            }
            FURI_LOG_I("GMS", "read - i: %d", read - i);
            uint16_t hid_id = game_macros_get_hid_pair(buffer + i);

            furi_check(hid_id);
            instruction[counter++].key.hid_key = hid_id;
            games_macros_read_flags_set_instruction_counter(&flags, set, counter);
            flags.reading_line = false;
        }
        read = storage_file_read(file, buffer, FILE_SIZE_MAX);
    }
    storage_file_close(file);
}

void game_macros_script_parcer(void* context, uint32_t index) {
    UNUSED(index);
    FURI_LOG_I("GMS", "Script parcer");
    GameMacrosScriptContext* ctx = context;
    ctx->ctx->map = malloc(sizeof(GameMacrosScriptMapping));
    GameMacrosScriptMapping* map = ctx->ctx->map;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    FuriString* file_path = furi_string_alloc();
    FURI_LOG_I("GMS", "Script parcer, got the file path");
    const char* path = furi_string_get_cstr(ctx->path);

    furi_string_printf(file_path, "%s/%s", path, "back.txt");
    game_macros_script_read_file(file, furi_string_get_cstr(file_path), &map->back);

    furi_string_printf(file_path, "%s/%s", path, "down.txt");
    game_macros_script_read_file(file, furi_string_get_cstr(file_path), &map->down);

    furi_string_printf(file_path, "%s/%s", path, "left.txt");
    game_macros_script_read_file(file, furi_string_get_cstr(file_path), &map->left);

    furi_string_printf(file_path, "%s/%s", path, "ok.txt");
    game_macros_script_read_file(file, furi_string_get_cstr(file_path), &map->ok);

    furi_string_printf(file_path, "%s/%s", path, "right.txt");
    game_macros_script_read_file(file, furi_string_get_cstr(file_path), &map->right);

    furi_string_printf(file_path, "%s/%s", path, "up.txt");
    game_macros_script_read_file(file, furi_string_get_cstr(file_path), &map->up);

    FURI_LOG_I("GMS", "Script parcer, files read");
    furi_string_free(file_path);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    game_macros_view_control_set_map(ctx->ctx->control, map);
    FURI_LOG_I("GMS", "Script parcer, view_dispatcher_switch");
    ctx->ctx->view_id = GameMacrosControlView;
    view_dispatcher_switch_to_view(ctx->ctx->view_dispatcher, GameMacrosControlView);
}