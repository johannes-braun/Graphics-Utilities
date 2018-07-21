#pragma once

#include "../descriptor.hpp"

namespace gfx {
inline namespace v1 {
namespace opengl {
class descriptor_set_implementation : public detail::descriptor_set_implementation
{
public:
    void     update(descriptor_type type, uint32_t binding, std::initializer_list<std::any> infos) override {}
    std::any api_handle() override { /* stub */ return {}; }
};

}    // namespace opengl
}    // namespace v1
}    // namespace gfx