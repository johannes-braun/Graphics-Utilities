#pragma once

#include "image_view.hpp"
#include <gfx/api.hpp>

// TODO
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
enum class attachment
{
    color,
    resolve,
    depth_stencil
};
struct depth_stencil
{
    float    depth;
    uint32_t stencil;
};
using clear_value = std::variant<glm::vec4, depth_stencil>;

class commands;
class framebuffer
{
    friend class commands;
    struct draw_buffer_settings
    {
        bool enable_color;
        bool enable_resolve;
    };

public:
    framebuffer(uint32_t width, uint32_t height) : _width(width), _height(height)
    {
        glCreateFramebuffers(1, &_msaa_fbo);
        glCreateFramebuffers(1, &_resolve_fbo);
    }

    ~framebuffer()
    {
        if (glIsFramebuffer(_msaa_fbo)) glDeleteFramebuffers(1, &_msaa_fbo);
        if (glIsFramebuffer(_resolve_fbo)) glDeleteFramebuffers(1, &_resolve_fbo);
    }

    void attach(attachment att, uint32_t index, const image_view& img_view, std::optional<clear_value> clear = {})
    {
        switch (att)
        {
        case attachment::color:
            glNamedFramebufferTexture(_msaa_fbo, GL_COLOR_ATTACHMENT0 + GLenum(index), handle_cast<mygl::texture>(img_view), 0);
            _enabled_drawbuffers.resize(std::max<size_t>(_enabled_drawbuffers.size(), index + 1));
            _color_clear_values.resize(std::max<size_t>(_color_clear_values.size(), index + 1));
            _enabled_drawbuffers[index].enable_color = true;
            _color_clear_values[index]               = std::move(clear);
            break;
        case attachment::resolve:
            glNamedFramebufferTexture(_resolve_fbo, GL_COLOR_ATTACHMENT0 + GLenum(index), handle_cast<mygl::texture>(img_view), 0);
            _enabled_drawbuffers.resize(std::max<size_t>(_enabled_drawbuffers.size(), index + 1));
            _enabled_drawbuffers[index].enable_resolve = true;
            break;
        case attachment::depth_stencil:
            glNamedFramebufferTexture(_msaa_fbo, GL_DEPTH_ATTACHMENT, handle_cast<mygl::texture>(img_view), 0);
            _depth_clear_value = std::move(clear);
            break;
        }
    }

    void detach(attachment att, uint32_t index)
    {
        switch (att)
        {
        case attachment::color:
            glNamedFramebufferTexture(_msaa_fbo, GL_COLOR_ATTACHMENT0 + GLenum(index), mygl::texture::zero, 0);
            _enabled_drawbuffers[index].enable_color = false;
            _color_clear_values[index]               = {};
            break;
        case attachment::resolve:
            glNamedFramebufferTexture(_resolve_fbo, GL_COLOR_ATTACHMENT0 + GLenum(index), mygl::texture::zero, 0);
            _enabled_drawbuffers[index].enable_resolve = false;
            break;
        case attachment::depth_stencil:
            glNamedFramebufferTexture(_msaa_fbo, GL_DEPTH_ATTACHMENT, mygl::texture::zero, 0);
            _depth_clear_value = {};
            break;
        }
    }

    void begin()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _msaa_fbo);

        // foreach drawbuffer ...
        _drawbuffers.resize(_enabled_drawbuffers.size());
        int buffers = 0;
        for (auto i = 0u; i < _enabled_drawbuffers.size(); ++i) {
            if (_enabled_drawbuffers[i].enable_color) _drawbuffers[buffers++] = GL_COLOR_ATTACHMENT0 + GLenum(i);
        }
        glNamedFramebufferDrawBuffers(_msaa_fbo, buffers, _drawbuffers.data());
    }

    void end()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mygl::framebuffer::zero);
        _resolved = 0;
        for (auto i = 0u; i < _enabled_drawbuffers.size(); ++i) {
            const auto drawbuffer = GL_COLOR_ATTACHMENT0 + GLenum(i);

            if (_enabled_drawbuffers[i].enable_resolve) {
                ++_resolved;
                glNamedFramebufferReadBuffer(_msaa_fbo, drawbuffer);
                glNamedFramebufferDrawBuffer(_resolve_fbo, drawbuffer);
                glBlitNamedFramebuffer(_msaa_fbo, _resolve_fbo, 0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT,
                                       GL_LINEAR);
            }
        }
    }

    std::any                                       api_handle() const { return _msaa_fbo; }
    mygl::framebuffer                              resolver() const { return _resolved > 0 ? _resolve_fbo : _msaa_fbo; }
    const std::vector<std::optional<clear_value>>& color_clear_values() const noexcept { return _color_clear_values; };
    const std::optional<clear_value>&              depth_clear_value() const noexcept { return _depth_clear_value; };

private:
    uint32_t                                _width;
    uint32_t                                _height;
    mygl::framebuffer                       _msaa_fbo;
    mygl::framebuffer                       _resolve_fbo;
    std::vector<GLenum>                     _drawbuffers;
    std::vector<draw_buffer_settings>       _enabled_drawbuffers;
    uint32_t                                _resolved;
    std::vector<std::optional<clear_value>> _color_clear_values;
    std::optional<clear_value>              _depth_clear_value;
};
}    // namespace v1
}    // namespace gfx