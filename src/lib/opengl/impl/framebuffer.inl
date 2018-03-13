#pragma once

namespace gl
{
    template<typename Container, typename>
    void framebuffer::set_drawbuffers(Container attachments) noexcept
    {
        _drawbuffers.clear();
        _drawbuffers.insert(_drawbuffers.begin(), attachments.begin(), attachments.end());
        glNamedFramebufferDrawBuffers(_id, _drawbuffers.size(), _drawbuffers.data());
    }
}