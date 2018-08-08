#pragma once

#include "../framebuffer.hpp"
#include "../general/handle.hpp"
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {
class commands_implementation;
class framebuffer_implementation : public detail::framebuffer_implementation
{
    friend class commands_implementation;

public:
    ~framebuffer_implementation();
    void   create(u32 width, u32 height, u32 layers, const renderpass_layout& layout) override;
    void   attach(attachment att, u32 index, const image_view& img_view, std::optional<clear_value> clear) override;
    void   detach(attachment att, u32 index) override;
    handle api_handle() override;

    void begin();
    void end();

private:
    struct draw_buffer_settings
    {
        bool enable_color;
        bool enable_resolve;
    };

    u32                               _width;
    u32                               _height;
    movable_handle<mygl::framebuffer> _msaa_fbo;
    movable_handle<mygl::framebuffer> _resolve_fbo;
    std::vector<GLenum>               _drawbuffers;
    std::vector<draw_buffer_settings> _enabled_drawbuffers;
    u32                               _resolved = 0;
};
}    // namespace opengl
}    // namespace v1
}    // namespace gfx