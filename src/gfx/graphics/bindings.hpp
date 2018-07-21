#pragma once
#include <cinttypes>

namespace gfx {
inline namespace v1 {
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
}    // namespace v1
}    // namespace gfx