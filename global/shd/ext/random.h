#pragma once

// Generates uniformly distributed random 2d sample points.
vec2 random_hammersley_2d(float current, float inverse_sample_count);

// Generates a random floating point value between 0 and 1 from a given seed.
float random_value(int seed);

#include "impl/random_impl.inl"