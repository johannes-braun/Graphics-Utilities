#pragma once

vec2 random_hammersley_2d(float current, float inverse_sample_count)
{
    vec2 result;
    result.x = current * inverse_sample_count;

    // Radical inverse
    bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	result.y = float(bits) * 2.3283064365386963e-10f;
    return result;
}

//Method to generate a pseudo-random seed.
uint random_wang_hash(uint a)
{
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
	a = a ^ (a >> 4);
	a = a * 0x27d4eb2d;
	a = a ^ (a >> 15);
	return a;
}

uint _rng_state = 0;
uint random_xorshift()
{
	// Xorshift algorithm from George Marsaglia's paper
	_rng_state ^= (_rng_state << 13);
	_rng_state ^= (_rng_state >> 17);
	_rng_state ^= (_rng_state << 5);
	return _rng_state;
}

const float inverse_max_uint = 1.f / float(0xFFFFFFFFu);
float random_value(int seed)
{
    _rng_state += random_wang_hash(uint(seed));
	return clamp(float(random_xorshift()) * inverse_max_uint, 0.f, 1.f);
}