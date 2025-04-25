#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fmt/base.h>
#include <limits>
#include <vector>

// Represents a Veilrator module that has a .clk signal
template <typename T>
concept ClockableModule = requires(T module) {
    module.eval();
};

struct ClockBase {
    virtual void tick() = 0;
    virtual void advance(uint32_t delta) = 0;
    virtual auto get_time_till_next_tick() const -> uint32_t = 0;
};

template <ClockableModule T> struct Clock : ClockBase {
    Clock(T *module, uint32_t pos_period, uint32_t neg_period = 0, bool should_start_posedge = false)
        : pos_period{pos_period}, neg_period{neg_period}, module(module), is_posedge{should_start_posedge} {
        assert(!(pos_period == 0u && neg_period == 0u));
    }

    void tick() override {
        time_since_last_tick = 0u;
        if constexpr(requires {module->clk();}) {
            *module->clk() = is_posedge;
        } else {
            module->clk = is_posedge;
        }
        module->eval();
        is_posedge = !is_posedge;
        if (current_period() == 0u) {
            tick();
        }
    }

    void advance(uint32_t delta) override {
        time_since_last_tick += delta;

        if (time_since_last_tick == current_period()) {
            tick();
            return;
        } else if (time_since_last_tick > current_period()) {
            fmt::println("Clock overshot by {} cycles", time_since_last_tick - current_period());
        }
    }

    auto get_time_till_next_tick() const -> uint32_t override { return current_period() - time_since_last_tick; }
    auto current_period() const -> uint32_t { return is_posedge ? pos_period : neg_period; }

    auto get_module_ptr() const -> const T* const {
        return module;
    }

    const uint32_t pos_period;
    const uint32_t neg_period;

  private:
    T *module;

    bool is_posedge;
    uint32_t time_since_last_tick = 0u;
};

struct ClockScheduler {
    void add_clock(ClockBase *clock) { clocks.push_back(clock); }

    void advance() {
        auto min_time = std::numeric_limits<uint32_t>::max();

        for (auto clock : clocks) {
            const auto time = clock->get_time_till_next_tick();
            min_time = std::min(min_time, time);
        }

        for (auto clock : clocks) {
            clock->advance(min_time);
        }
    }

    std::vector<ClockBase *> clocks;
};

