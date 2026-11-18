// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layers {
    BASE,
    SYM,
    NAV,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT_split_3x6_3(
        KC_TAB,  KC_Q, KC_W, KC_E, KC_R, KC_T,   KC_Y,   KC_U, KC_I,    KC_O,   KC_P,    KC_BSPC,
        KC_ESC,  KC_A, KC_S, KC_D, KC_F, KC_G,   KC_H,   KC_J, KC_K,    KC_L,   KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B,   KC_N,   KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_LGUI,
                            MO(SYM), KC_LCTL, KC_SPC,   KC_SPC, KC_LALT, MO(NAV)
    ),
    [SYM] = LAYOUT_split_3x6_3(
        KC_GRV,  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,   KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_BSLS, KC_PIPE,
        _______, KC_LPRN, KC_RPRN, KC_LBRC, KC_RBRC, _______,  _______, KC_LCBR, KC_RCBR, KC_LABK, KC_RABK, KC_DQUO,
        _______, KC_MINS, KC_UNDS, KC_EQL,  KC_PLUS, _______,  _______, KC_COLN, KC_QUES, _______, _______, _______,
                                            _______, _______, _______,   _______, _______, _______
    ),
    [NAV] = LAYOUT_split_3x6_3(
        _______, KC_7, KC_8, KC_9, _______, _______,   _______, KC_HOME, KC_UP,   KC_END,  KC_PGUP, KC_ENT,
        _______, KC_4, KC_5, KC_6, _______, _______,   _______, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, KC_DEL,
        _______, KC_1, KC_2, KC_3, KC_0,    _______,   _______, _______, _______, _______, _______, _______,
                                  _______, _______, _______,   _______, _______, _______
    ),
};
