// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layers {
    BASE,
    SYM,
    NAV,
    FN,
};

enum custom_keycodes {
    PUNCT_LINUX = SAFE_RANGE,
    PUNCT_WINDOWS,
    INV_QUESTION,
    INV_EXCLAMATION,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT_split_3x6_3(
        KC_ESC,  KC_Q, KC_W, KC_E, KC_R, KC_T,   KC_Y,   KC_U, KC_I,    KC_O,   KC_P,    KC_BSPC,
        KC_LSFT, KC_A, KC_S, KC_D, KC_F, KC_G,   KC_H,   KC_J, KC_K,    KC_L,   KC_SCLN, KC_RSFT,
        KC_LCTL, KC_Z, KC_X, KC_C, KC_V, KC_B,   KC_N,   KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_QUOT,
                KC_LGUI, LT(SYM, KC_TAB), KC_SPC,   KC_SPC, LT(NAV, KC_ENT), KC_LALT
    ),
    [SYM] = LAYOUT_split_3x6_3(
        _______, KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,   KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_BSLS, KC_PIPE,
        KC_LGUI, KC_LPRN, KC_RPRN, KC_LBRC, KC_RBRC, _______,  KC_GRV,  KC_LCBR, KC_RCBR, KC_LABK, KC_RABK, _______,
        _______, KC_MINS, KC_UNDS, KC_EQL,  KC_PLUS, _______,  _______, KC_COLN, KC_QUES, INV_QUESTION, INV_EXCLAMATION, KC_DQUO,
                                            _______, _______, _______,   _______, _______, _______
    ),
    [NAV] = LAYOUT_split_3x6_3(
        _______, KC_7, KC_8, KC_9, _______, PUNCT_WINDOWS,   _______, KC_HOME, KC_UP,   KC_END,  KC_PGUP, _______,
        _______, KC_4, KC_5, KC_6, _______, _______,   _______, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, KC_LALT,
        _______, KC_1, KC_2, KC_3, KC_0,    PUNCT_LINUX,   _______, _______, _______, _______, _______, _______,
                                  _______, _______, _______,   _______, _______, _______
    ),
    [FN] = LAYOUT_split_3x6_3(
        KC_F1, KC_F2,   KC_F3,   KC_F4,   KC_F5, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10, KC_F11, KC_F12,
        KC_NO, KC_MUTE, KC_VOLD, KC_VOLU, KC_NO, KC_NO,   KC_MPRV, KC_MPLY, KC_MNXT, KC_NO,  KC_NO,  KC_PSCR,
        KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,  KC_NO,  KC_NO,
                               _______, _______, _______,   _______, _______, _______
    ),
};

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, SYM, NAV, FN);
}

const key_override_t delete_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);

const key_override_t *key_overrides[] = {
    &delete_key_override,
};

enum { CAPS_TAP_MS = 200 };

static bool     punct_windows;
static bool     caps_candidate;
static bool     caps_pending;
static uint32_t caps_pressed_at;
static uint32_t caps_released_at;

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
    if (keycode == KC_LSFT && record->event.pressed) {
        caps_candidate = get_highest_layer(layer_state) == BASE && !get_mods() && !has_anykey();
        caps_pending = caps_candidate && caps_pending && timer_elapsed32(caps_released_at) < CAPS_TAP_MS;
        caps_pressed_at = timer_read32();
    } else if (keycode != KC_LSFT) {
        caps_candidate = false;
        caps_pending = false;
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
    if (keycode == INV_QUESTION || keycode == INV_EXCLAMATION) {
        if (record->event.pressed && !get_mods()) {
            send_inverted_punctuation(keycode == INV_QUESTION);
        }
        return false;
    }
    return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode != KC_LSFT || record->event.pressed) {
        return;
    }

    // Send Caps only after QMK has released the physical Shift.
    if (!caps_candidate || timer_elapsed32(caps_pressed_at) >= CAPS_TAP_MS || get_mods() || has_anykey()) {
        caps_pending = false;
    } else if (host_keyboard_led_state().caps_lock || caps_pending) {
        caps_pending = false;
        tap_code16(KC_CAPS);
    } else {
        caps_pending = true;
        caps_released_at = timer_read32();
    }
    caps_candidate = false;
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
        case FN:
            rgb_matrix_set_color_all(0, 32, 0);
            break;
        default:
            rgb_matrix_set_color_all(16, 0, 32);
            break;
    }
    return true;
}
#endif
