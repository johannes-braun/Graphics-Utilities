#pragma once
#include <gfx.ecs/ecs.hpp>
#include <atomic>

using namespace std::chrono_literals;

struct ecs_state_t
{
    gfx::ecs::system_list physics_list;
    gfx::ecs::system_list inputs_list;
    gfx::ecs::ecs         ecs;

    std::mutex glfw_mutex;
};

constexpr auto update_time_graphics = 8ms;
constexpr auto update_time_physics  = 8ms;
constexpr auto update_time_inputs   = 8ms;