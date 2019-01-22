#pragma once
#include <chrono>

using namespace std::chrono_literals;

constexpr auto update_time_graphics = 0ms;
constexpr auto update_time_physics  = 8ms;
constexpr auto update_time_inputs   = 8ms;

constexpr auto arbitrary_bloom_count_which_i_don_t_know_the_purpose_of = 8;