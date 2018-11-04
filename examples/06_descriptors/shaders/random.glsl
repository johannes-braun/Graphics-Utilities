//? #version 460 core

const float inverse_max_uint = 1.f / float(0xFFFFFFFFu);
const float random_primes[6] = {69019.f, 96013.f, 32159.f, 22783.f, 87011.f, 45263.f};

uint _rng_state     = 0;
int  _current_prime = 0;

uint random_wang_hash(uint a);
uint random_xorshift();

float random_value(int seed)
{
    _rng_state += random_wang_hash(uint(seed));
    return clamp(float(random_xorshift()) * inverse_max_uint, 0.f, 1.f);
}
float init_random(ivec2 pixel, float seed)
{
    return random_value(int(pixel.x * random_primes[0] + pixel.y * random_primes[1] + random_primes[2] * seed));
}
float next_random()
{
    return random_value(_current_prime = (_current_prime + 1) % 6);
}

// Method to generate a pseudo-random seed.
uint random_wang_hash(uint a)
{
    a = (a ^ 61) ^ (a >> 16);

    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}

uint random_xorshift()
{
    // Xorshift algorithm from George Marsaglia's paper
    _rng_state ^= (_rng_state << 13);
    _rng_state ^= (_rng_state >> 17);
    _rng_state ^= (_rng_state << 5);
    return _rng_state;
}

vec2 random_hammersley_2d(float current, float inverse_sample_count)
{
    vec2 result;
    result.x = current * inverse_sample_count;

    // Radical inverse
    uint bits = uint(current);
    bits      = (bits << 16u) | (bits >> 16u);
    bits      = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits      = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits      = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits      = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    result.y  = float(bits) * 2.3283064365386963e-10f;
    return result;
}