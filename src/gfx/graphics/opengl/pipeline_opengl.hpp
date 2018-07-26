#pragma once
#define GFX_EXPOSE_APIS
#include "../general/handle.hpp"
#include <gfx/graphics/pipeline.hpp>
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {
struct shader_module
{
    movable_handle<mygl::shader_program> s;
    ~shader_module();
};

inline GLbitfield stage_bitfield(GLenum st) noexcept;
auto              make_shader(shader_type t, shader& s, mygl::pipeline handle);
bool              validate(mygl::pipeline handle);

class graphics_pipeline_implementation : public detail::graphics_pipeline_implementation
{
public:
    ~graphics_pipeline_implementation();

    bool bind(vertex_input& input, state_info& info, const std::vector<std::pair<shader_type, std::shared_ptr<shader>>>& shaders,
              bool shaders_valid) override;

    handle api_handle() override;

private:
    std::vector<shader_module>     shds;
    movable_handle<mygl::pipeline> _handle = mygl::pipeline::zero;
};


class compute_pipeline_implementation : public detail::compute_pipeline_implementation
{
public:
    bool   build(shader& shaders) override;
    handle api_handle() override;

    ~compute_pipeline_implementation() override;

private:
    shader_module                  _s      = {mygl::shader_program::zero};
    movable_handle<mygl::pipeline> _handle = mygl::pipeline::zero;
};

}    // namespace opengl
}    // namespace v1
}    // namespace gfx