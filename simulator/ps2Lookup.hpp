#pragma once

#include <variant>
#include <raylib.h>
#include <cstdint>

auto key_to_scan_code(KeyboardKey key) -> std::variant<uint8_t, std::pair<uint8_t, uint8_t>> {
    switch (key) {
        case KEY_APOSTROPHE:
            return static_cast<uint8_t>(0x52);
        case KEY_COMMA:
            return static_cast<uint8_t>(0x41);
        case KEY_MINUS:
            return static_cast<uint8_t>(0x4E);
        case KEY_PERIOD:
            return static_cast<uint8_t>(0x49);
        case KEY_SLASH:
            return static_cast<uint8_t>(0x4A);
        case KEY_ZERO:
            return static_cast<uint8_t>(0x70);
        case KEY_ONE:
            return static_cast<uint8_t>(0x16);
        case KEY_TWO:
            return static_cast<uint8_t>(0x1E);
        case KEY_THREE:
            return static_cast<uint8_t>(0x26);
        case KEY_FOUR:
            return static_cast<uint8_t>(0x25);
        case KEY_FIVE:
            return static_cast<uint8_t>(0x2E);
        case KEY_SIX:
            return static_cast<uint8_t>(0x36);
        case KEY_SEVEN:
            return static_cast<uint8_t>(0x3D);
        case KEY_EIGHT:
            return static_cast<uint8_t>(0x3E);
        case KEY_NINE:
            return static_cast<uint8_t>(0x46);
        case KEY_SEMICOLON:
            return static_cast<uint8_t>(0x4C);
        case KEY_EQUAL:
            return static_cast<uint8_t>(0x55);
        case KEY_A:
            return static_cast<uint8_t>(0x1C);
        case KEY_B:
            return static_cast<uint8_t>(0x32);
        case KEY_C:
            return static_cast<uint8_t>(0x21);
        case KEY_D:
            return static_cast<uint8_t>(0x23);
        case KEY_E:
            return static_cast<uint8_t>(0x24);
        case KEY_F:
            return static_cast<uint8_t>(0x2B);
        case KEY_G:
            return static_cast<uint8_t>(0x34);
        case KEY_H:
            return static_cast<uint8_t>(0x33);
        case KEY_I:
            return static_cast<uint8_t>(0x43);
        case KEY_J:
            return static_cast<uint8_t>(0x3B);
        case KEY_K:
            return static_cast<uint8_t>(0x42);
        case KEY_L:
            return static_cast<uint8_t>(0x4B);
        case KEY_M:
            return static_cast<uint8_t>(0x3A);
        case KEY_N:
            return static_cast<uint8_t>(0x31);
        case KEY_O:
            return static_cast<uint8_t>(0x44);
        case KEY_P:
            return static_cast<uint8_t>(0x4D);
        case KEY_Q:
            return static_cast<uint8_t>(0x15);
        case KEY_R:
            return static_cast<uint8_t>(0x2D);
        case KEY_S:
            return static_cast<uint8_t>(0x1B);
        case KEY_T:
            return static_cast<uint8_t>(0x2C);
        case KEY_U:
            return static_cast<uint8_t>(0x3C);
        case KEY_V:
            return static_cast<uint8_t>(0x2A);
        case KEY_W:
            return static_cast<uint8_t>(0x1D);
        case KEY_X:
            return static_cast<uint8_t>(0x22);
        case KEY_Y:
            return static_cast<uint8_t>(0x35);
        case KEY_Z:
            return static_cast<uint8_t>(0x1A);
        case KEY_LEFT_BRACKET:
            return static_cast<uint8_t>(0x54);
        case KEY_BACKSLASH:
            return static_cast<uint8_t>(0x5D);
        case KEY_RIGHT_BRACKET:
            return static_cast<uint8_t>(0x5B);
        case KEY_GRAVE:
            return static_cast<uint8_t>(0x0E);
        case KEY_SPACE:
            return static_cast<uint8_t>(0x29);
        case KEY_ESCAPE:
            return static_cast<uint8_t>(0x76);
        case KEY_ENTER:
            return static_cast<uint8_t>(0x5A);
        case KEY_TAB:
            return static_cast<uint8_t>(0x0D);
        case KEY_BACKSPACE:
            return static_cast<uint8_t>(0x66);
        case KEY_INSERT:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x70);
        case KEY_DELETE:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x71);
        case KEY_RIGHT:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x74);
        case KEY_LEFT:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x6B);
        case KEY_DOWN:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x72);
        case KEY_UP:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x75);
        case KEY_PAGE_UP:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x7D);
        case KEY_PAGE_DOWN:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x7A);
        case KEY_HOME:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x6C);
        case KEY_END:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x69);
        case KEY_CAPS_LOCK:
            return static_cast<uint8_t>(0x58);
        case KEY_SCROLL_LOCK:
            return static_cast<uint8_t>(0x7E);
        case KEY_NUM_LOCK:
            return static_cast<uint8_t>(0x77);
        case KEY_F1:
            return static_cast<uint8_t>(0x05);
        case KEY_F2:
            return static_cast<uint8_t>(0x06);
        case KEY_F3:
            return static_cast<uint8_t>(0x04);
        case KEY_F4:
            return static_cast<uint8_t>(0x0C);
        case KEY_F5:
            return static_cast<uint8_t>(0x03);
        case KEY_F6:
            return static_cast<uint8_t>(0x0B);
        case KEY_F7:
            return static_cast<uint8_t>(0x83);
        case KEY_F8:
            return static_cast<uint8_t>(0x0A);
        case KEY_F9:
            return static_cast<uint8_t>(0x01);
        case KEY_F10:
            return static_cast<uint8_t>(0x09);
        case KEY_F11:
            return static_cast<uint8_t>(0x78);
        case KEY_F12:
            return static_cast<uint8_t>(0x07);
        case KEY_LEFT_SHIFT:
            return static_cast<uint8_t>(0x12);
        case KEY_LEFT_CONTROL:
            return static_cast<uint8_t>(0x14);
        case KEY_LEFT_ALT:
            return static_cast<uint8_t>(0x11);
        case KEY_LEFT_SUPER:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x1F);
        case KEY_RIGHT_SHIFT:
            return static_cast<uint8_t>(0x59);
        case KEY_RIGHT_CONTROL:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x14);
        case KEY_RIGHT_ALT:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x11);
        case KEY_RIGHT_SUPER:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x27);
        case KEY_KB_MENU:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x2F);
        case KEY_KP_0:
            return static_cast<uint8_t>(0x70);
        case KEY_KP_1:
            return static_cast<uint8_t>(0x69);
        case KEY_KP_2:
            return static_cast<uint8_t>(0x72);
        case KEY_KP_3:
            return static_cast<uint8_t>(0x7A);
        case KEY_KP_4:
            return static_cast<uint8_t>(0x6B);
        case KEY_KP_5:
            return static_cast<uint8_t>(0x73);
        case KEY_KP_6:
            return static_cast<uint8_t>(0x74);
        case KEY_KP_7:
            return static_cast<uint8_t>(0x6C);
        case KEY_KP_8:
            return static_cast<uint8_t>(0x75);
        case KEY_KP_9:
            return static_cast<uint8_t>(0x7D);
        case KEY_KP_DECIMAL:
            return static_cast<uint8_t>(0x71);
        case KEY_KP_DIVIDE:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x4A);
        case KEY_KP_MULTIPLY:
            return static_cast<uint8_t>(0x7C);
        case KEY_KP_SUBTRACT:
            return static_cast<uint8_t>(0x7B);
        case KEY_KP_ADD:
            return static_cast<uint8_t>(0x79);
        case KEY_KP_ENTER:
            return std::pair<uint8_t, uint8_t>(0xE0, 0x5A);
        default:
            return static_cast<uint8_t>(0x00);
    }
}