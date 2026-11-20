// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layers {
    BASE,
    SYM,
    NAV,
};

enum custom_keycodes {
    PUNCT_LINUX = SAFE_RANGE,
    PUNCT_WINDOWS,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT_split_3x6_3(
        KC_ESC,  KC_Q, KC_W, KC_E, KC_R, KC_T,   KC_Y,   KC_U, KC_I,    KC_O,   KC_P,    KC_BSPC,
        KC_LSFT, KC_A, KC_S, KC_D, KC_F, KC_G,   KC_H,   KC_J, KC_K,    KC_L,   KC_SCLN, KC_RSFT,
        KC_LCTL, KC_Z, KC_X, KC_C, KC_V, KC_B,   KC_N,   KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_QUOT,
                KC_LGUI, LT(SYM, KC_TAB), KC_SPC,   KC_SPC, LT(NAV, KC_ENT), KC_LALT
    ),
    [SYM] = LAYOUT_split_3x6_3(
        KC_ENT,  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,   KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_BSLS, KC_PIPE,
        KC_LGUI, KC_LPRN, KC_RPRN, KC_LBRC, KC_RBRC, _______,  KC_GRV,  KC_LCBR, KC_RCBR, KC_LABK, KC_RABK, _______,
        _______, KC_MINS, KC_UNDS, KC_EQL,  KC_PLUS, _______,  _______, KC_COLN, KC_QUES, _______, _______, KC_DQUO,
                                            _______, _______, _______,   _______, _______, _______
    ),
    [NAV] = LAYOUT_split_3x6_3(
        _______, KC_7, KC_8, KC_9, _______, PUNCT_WINDOWS,   _______, KC_HOME, KC_UP,   KC_END,  KC_PGUP, _______,
        _______, KC_4, KC_5, KC_6, _______, _______,   _______, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, KC_LALT,
        _______, KC_1, KC_2, KC_3, KC_0,    PUNCT_LINUX,   _______, _______, _______, _______, _______, KC_CAPS,
                                  _______, _______, _______,   _______, _______, _______
    ),
};

const key_override_t delete_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);

const key_override_t *key_overrides[] = {
    &delete_key_override,
};

enum { SHIFT_TAP_MS = 200 };

static bool     punct_windows;
static uint16_t held_shift;
static uint16_t tap_time;
static uint8_t  shift_taps;
static bool     tap_down;

static void send_inverted_punctuation(bool question) {
    if (punct_windows) {
        const bool num_lock = host_keyboard_led_state().num_lock;
        if (!num_lock) {
            tap_code_delay(KC_NUM, 10);
            wait_ms(10);
        }
        register_code(KC_LALT);
        tap_code_delay(KC_P0, 5);
        tap_code_delay(KC_P1, 5);
        tap_code_delay(question ? KC_P9 : KC_P6, 5);
        tap_code_delay(KC_P1, 5);
        unregister_code(KC_LALT);
        if (!num_lock) {
            tap_code_delay(KC_NUM, 10);
        }
    } else {
        tap_code16(C(S(KC_U)));
        wait_ms(10);
        tap_code_delay(question ? KC_B : KC_A, 5);
        tap_code_delay(question ? KC_F : KC_1, 5);
        tap_code(KC_ENT);
    }
}

bool pre_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed && keycode != KC_LSFT && keycode != KC_RSFT) {
        held_shift = KC_NO;
    }
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == PUNCT_LINUX || keycode == PUNCT_WINDOWS) {
        if (record->event.pressed) {
            punct_windows = keycode == PUNCT_WINDOWS;
        }
        return false;
    }
    return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode != KC_LSFT && keycode != KC_RSFT) {
        return;
    }

    const uint16_t now = record->event.time;
    if (held_shift == KC_NO) {
        if (record->event.pressed && get_mods() == MOD_BIT(keycode)) {
            held_shift = keycode;
            shift_taps = 0;
            tap_down   = false;
        }
    } else if (keycode == held_shift) {
        const bool question = held_shift == KC_LSFT;
        held_shift = KC_NO;
        // Emit only after QMK has released the regular Shift modifier.
        if (!record->event.pressed && !tap_down && get_mods() == 0) {
            if (shift_taps == 1) {
                tap_code16(question ? S(KC_SLSH) : S(KC_1));
            } else if (shift_taps == 2) {
                send_inverted_punctuation(question);
            }
        }
    } else if (record->event.pressed) {
        if (shift_taps == 2 || (shift_taps == 1 && TIMER_DIFF_16(now, tap_time) >= SHIFT_TAP_MS)) {
            held_shift = KC_NO;
        } else {
            tap_down = true;
            tap_time = now;
        }
    } else if (tap_down && TIMER_DIFF_16(now, tap_time) < SHIFT_TAP_MS) {
        tap_down = false;
        tap_time = now;
        ++shift_taps;
    } else {
        held_shift = KC_NO;
    }
}

#ifdef RGB_MATRIX_ENABLE
void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
}

bool rgb_matrix_indicators_user(void) {
    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color_all(32, 0, 0);
        return true;
    }

    switch (get_highest_layer(layer_state)) {
        case SYM:
            rgb_matrix_set_color_all(32, 16, 0);
            break;
        case NAV:
            rgb_matrix_set_color_all(0, 0, 32);
            break;
        default:
            rgb_matrix_set_color_all(16, 0, 32);
            break;
    }
    return true;
}
#endif
