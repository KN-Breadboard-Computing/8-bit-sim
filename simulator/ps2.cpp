#include <ranges>
#include "ps2Lookup.hpp"
#include "ps2.hpp"

namespace stdr = std::ranges;

ps2::Packet::Packet(uint8_t value) : data({}) {
    bool parity = true;
    data.front() = 0;
    data.back() = 1;
    for (unsigned i: stdr::iota_view(0, 8)) {
        if (value >> (7 - i) & 0x01) {
            data.at(i+1) = 1;
            parity = !parity;
        }
    }
    data.at(9) = parity;
}

auto ps2::encode_key(KeyboardKey key, bool isRelease) -> std::optional<std::vector<Packet>> {
    std::vector<Packet> result;
    if (key == KEY_PRINT_SCREEN) {
        if (isRelease) {
            result.insert(result.end(),
                          {Packet(0xE0), Packet(0xF0), Packet(0x7C), Packet(0xE0), Packet(0xF0), Packet(0x12)});
        } else {
            result.insert(result.end(), {Packet(0xE0), Packet(0x12), Packet(0xE0), Packet(0x7C)});
        }
        return result;
    }
    if (key == KEY_PAUSE) {
        if (isRelease) {
            return {};
        }
        result.insert(result.end(),
                      {Packet(0xE1), Packet(0x14), Packet(0x77), Packet(0xE1), Packet(0xF0), Packet(0x14), Packet(0xE0),
                       Packet(0x77)});
        return result;
    }
    auto ret = key_to_scan_code(key);
    if (auto *val = std::get_if<uint8_t>(&ret)) {
        if (*val == 0) {
            return {};
        }
        if (isRelease) {
            result.emplace_back(0xF0);
        }
        result.emplace_back(*val);
        return result;
    }
    auto val = std::get<std::pair<uint8_t, uint8_t>>(ret);
    result.emplace_back(val.first);
    if (isRelease) {
        result.emplace_back(0xF0);
    }
    result.emplace_back(val.second);
    return result;
}
