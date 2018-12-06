#pragma once
#include <gfx.ecs/ecs.hpp>
#include <atomic>

using namespace std::chrono_literals;

struct ecs_state_t
{
    gfx::ecs::ecs         ecs;
    gfx::ecs::system_list physics_list;
    gfx::ecs::system_list inputs_list;
};

constexpr auto update_time_graphics = 0ms;
constexpr auto update_time_physics  = 10ms;
constexpr auto update_time_inputs   = 8ms;