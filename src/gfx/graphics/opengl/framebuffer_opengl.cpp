#include "framebuffer_opengl.hpp"

namespace gfx {
inline namespace v1 {
namespace opengl {

framebuffer_implementation::~framebuffer_implementation()
{
    if (glIsFramebuffer(_msaa_fbo)) glDeleteFramebuffers(1, &_msaa_fbo);
    if (glIsFramebuffer(_resolve_fbo)) glDeleteFramebuffers(1, &_resolve_fbo);
}

void framebuffer_implementation::create(u32 width, u32 height, u32 layers, const v2::renderpass_layout& layout)
{
    this->~framebuffer_implementation();
    glCreateFramebuffers(1, &_msaa_fbo);
    glCreateFramebuffers(1, &_resolve_fbo);
    _width  = width;
    _height = height;
}

void framebuffer_implementation::attach(attachment att, u32 index, const image_view& img_view, std::optional<clear_value> clear)
{
    switch (att)
    {
    case attachment::color:
        glNamedFramebufferTexture(_msaa_fbo, GL_COLOR_ATTACHMENT0 + GLenum(index), handle_cast<mygl::texture>(img_view), 0);
        _enabled_drawbuffers.resize(std::max<size_t>(_enabled_drawbuffers.size(), index + 1));
        _enabled_drawbuffers[index].enable_color = true;
        break;
    case attachment::resolve:
        glNamedFramebufferTexture(_resolve_fbo, GL_COLOR_ATTACHMENT0 + GLenum(index), handle_cast<mygl::texture>(img_view), 0);
        _enabled_drawbuffers.resize(std::max<size_t>(_enabled_drawbuffers.size(), index + 1));
        _enabled_drawbuffers[index].enable_resolve = true;
        break;
    case attachment::depth_stencil:
        glNamedFramebufferTexture(_msaa_fbo, GL_DEPTH_ATTACHMENT, handle_cast<mygl::texture>(img_view), 0);
        break;
    }
}

void framebuffer_implementation::detach(attachment att, u32 index)
{
    switch (att)
    {
    case attachment::color:
        glNamedFramebufferTexture(_msaa_fbo, GL_COLOR_ATTACHMENT0 + GLenum(index), mygl::texture::zero, 0);
        _enabled_drawbuffers[index].enable_color = false;
        break;
    case attachment::resolve:
        glNamedFramebufferTexture(_resolve_fbo, GL_COLOR_ATTACHMENT0 + GLenum(index), mygl::texture::zero, 0);
        _enabled_drawbuffers[index].enable_resolve = false;
        break;
    case attachment::depth_stencil: glNamedFramebufferTexture(_msaa_fbo, GL_DEPTH_ATTACHMENT, mygl::texture::zero, 0); break;
    }
}

handle framebuffer_implementation::api_handle()
{
    return _msaa_fbo.get();
}

void framebuffer_implementation::begin()
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

void framebuffer_implementation::end()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mygl::framebuffer::zero);
    _resolved = 0;
    for (auto i = 0u; i < _enabled_drawbuffers.size(); ++i) {
        const auto drawbuffer = GL_COLOR_ATTACHMENT0 + GLenum(i);

        if (_enabled_drawbuffers[i].enable_resolve) {
            ++_resolved;
            glNamedFramebufferReadBuffer(_msaa_fbo, drawbuffer);
            glNamedFramebufferDrawBuffer(_resolve_fbo, drawbuffer);
            glBlitNamedFramebuffer(_msaa_fbo, _resolve_fbo, 0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        }
    }
}
}    // namespace opengl
}    // namespace v1
}    // namespace gfx