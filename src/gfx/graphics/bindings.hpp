#pragma once
#include <cinttypes>

namespace gfx
{
enum class binding_target
{
    uniform_buffer,
    storage_buffer,
    storage_image,
    sampled_image,
};

class binding_layout
{
public:
    void add(binding_target target, uint32_t first, uint32_t count); 

private:
};
}