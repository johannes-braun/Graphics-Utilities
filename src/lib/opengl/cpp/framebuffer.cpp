#include "../framebuffer.hpp"
#include "../texture.hpp"

namespace gl
{
    renderbuffer::renderbuffer(GLenum internal_format, int width, int height) noexcept
        : _internal_format(internal_format), _width(width), _height(height), _multisampled(true)
    {
        glCreateRenderbuffers(1, &_id);
        glNamedRenderbufferStorage(_id, _internal_format, _width, _height);
    }

    renderbuffer::renderbuffer(GLenum internal_format, int width, int height, samples s) noexcept
        : _internal_format(internal_format), _width(width), _height(height), _samples(s), _multisampled(false)
    {
        glCreateRenderbuffers(1, &_id);
        glNamedRenderbufferStorageMultisample(_id, 1 << uint32_t(_samples), _internal_format, _width, _height);
    }

    renderbuffer::renderbuffer(const renderbuffer& other) noexcept
    {
        operator=(std::forward<const renderbuffer&>(other));
    }

    renderbuffer::renderbuffer(renderbuffer&& other) noexcept
    {
        operator=(std::forward<renderbuffer&&>(other));
    }

    renderbuffer& renderbuffer::operator=(const renderbuffer& other) noexcept
    {
        if (_id != gl_renderbuffer_t::zero)
            glDeleteRenderbuffers(1, &_id);
        glCreateRenderbuffers(1, &_id);

        _width = other._width;
        _height = other._height;
        _internal_format = other._internal_format;
        _samples = other._samples;
        _multisampled = other._multisampled;

        if (_multisampled)
            glNamedRenderbufferStorageMultisample(_id, 1 << uint32_t(_samples), _internal_format, _width, _height);
        else
            glNamedRenderbufferStorage(_id, _internal_format, _width, _height);

        return *this;
    }

    renderbuffer& renderbuffer::operator=(renderbuffer&& other) noexcept
    {
        _id = other._id;
        _width = other._width;
        _height = other._height;
        _internal_format = other._internal_format;
        _samples = other._samples;
        _multisampled = other._multisampled;
        other._id = gl_renderbuffer_t::zero;
        return *this;
    }

    renderbuffer::~renderbuffer() noexcept
    {
        if (_id != gl_renderbuffer_t::zero)
            glDeleteRenderbuffers(1, &_id);
    }

    renderbuffer::operator gl_renderbuffer_t() const noexcept
    {
        return _id;
    }

    int renderbuffer::width() const noexcept
    {
        return _width;
    }

    int renderbuffer::height() const noexcept
    {
        return _height;
    }

    std::nullptr_t attachment::set(std::nullptr_t)
    {
        _level = 0;
        _layer = 0;
        _layered = false;
        _storage = nullptr;
        _attachment_type = attachment_type::empty;
        glNamedFramebufferTexture(_framebuffer, _type, gl_texture_t::zero, 0);
        return nullptr;
    }

    std::shared_ptr<renderbuffer>& attachment::set(const std::shared_ptr<renderbuffer>& r)
    {
        _level = 0;
        _layer = 0;
        _layered = false;
        _attachment_type = attachment_type::renderbuffer;
        glNamedFramebufferRenderbuffer(_framebuffer, _type, GL_RENDERBUFFER, *r);
        return std::get<std::shared_ptr<renderbuffer>>(_storage = r);
    }

    std::shared_ptr<texture>& attachment::set(const std::shared_ptr<texture>& t)
    {
        return set(t, 0);
    }

    std::shared_ptr<texture>& attachment::set(const std::shared_ptr<texture>& t, int level)
    {
        _level = level;
        _layer = 0;
        _layered = false;
        _attachment_type = attachment_type::texture;
        glNamedFramebufferTexture(_framebuffer, _type, *t, level);
        return std::get<std::shared_ptr<texture>>(_storage = t);
    }

    std::shared_ptr<texture>& attachment::set(const std::shared_ptr<texture>& t, int level, int layer)
    {
        _level = level;
        _layer = layer;
        _layered = true;
        _attachment_type = attachment_type::texture;
        glNamedFramebufferTextureLayer(_framebuffer, _type, *t, level, layer);
        return std::get<std::shared_ptr<texture>>(_storage = t);
    }

    std::shared_ptr<texture>& attachment::operator=(const std::shared_ptr<texture>& t)
    {
        return set(t);
    }

    std::shared_ptr<renderbuffer>& attachment::operator=(const std::shared_ptr<renderbuffer>& r)
    {
        return set(r);
    }

    std::nullptr_t attachment::operator=(std::nullptr_t)
    {
        return set(nullptr);
    }

    attachment::operator const std::shared_ptr<texture>&() const
    {
        static std::shared_ptr<texture> null{ nullptr };
        return _attachment_type == attachment_type::texture ? std::get<std::shared_ptr<texture>>(_storage) : null;
    }

    attachment::operator const std::shared_ptr<renderbuffer>&() const
    {
        static std::shared_ptr<renderbuffer> null{ nullptr };
        return _attachment_type == attachment_type::renderbuffer ? std::get<std::shared_ptr<renderbuffer>>(_storage) : null;
    }

    attachment::attachment(const framebuffer& framebuffer, GLenum type)
        : _framebuffer(framebuffer), _type(type), _storage(nullptr)
    {

    }

    int attachment::width() const noexcept
    {
        switch (_attachment_type)
        {
        case attachment_type::texture: return std::get<std::shared_ptr<texture>>(_storage)->width();
        case attachment_type::renderbuffer: return std::get<std::shared_ptr<renderbuffer>>(_storage)->width();
        default: return 0;
        }
    }

    int attachment::height() const noexcept
    {
        switch (_attachment_type)
        {
        case attachment_type::texture: return std::get<std::shared_ptr<texture>>(_storage)->height();
        case attachment_type::renderbuffer: return std::get<std::shared_ptr<renderbuffer>>(_storage)->height();
        default: return 0;
        }
    }

    framebuffer& framebuffer::zero()
    {
        static framebuffer def(nullptr);
        return def;
    }

    framebuffer::framebuffer(std::nullptr_t)
        : _id(gl_framebuffer_t::zero)
    {

    }

    framebuffer::framebuffer() noexcept
    {
        glCreateFramebuffers(1, &_id);
    }

    framebuffer::framebuffer(const framebuffer& other) noexcept
    {
        operator=(std::forward<const framebuffer&>(other));
    }

    framebuffer::framebuffer(framebuffer&& other) noexcept
    {
        operator=(std::forward<framebuffer&&>(other));
    }

    framebuffer& framebuffer::operator=(const framebuffer& other) noexcept
    {
        if (_id != gl_framebuffer_t::zero)
            glDeleteFramebuffers(1, &_id);
        if (other._id == gl_framebuffer_t::zero)
            _id = gl_framebuffer_t::zero;
        else
            glCreateFramebuffers(1, &_id);

        for (auto&& a : other._attachments)
        {
            const auto& texture_ptr = a.second.operator const std::shared_ptr<texture> &();
            const auto& renderbuffer_ptr = a.second.operator const std::shared_ptr<renderbuffer> &();

            _attachments.emplace(a.first, attachment(*this, a.first));

            if (texture_ptr && a.second._layered)
                _attachments.at(a.first).set(texture_ptr, a.second._level, a.second._layer);
            else if (texture_ptr)
                _attachments.at(a.first).set(texture_ptr, a.second._level);
            else if (renderbuffer_ptr)
                _attachments.at(a.first).set(renderbuffer_ptr);
            else
                _attachments.at(a.first).set(nullptr);
        }

        set_readbuffer(other._readbuffer);
        set_drawbuffers(other._drawbuffers);

        return *this;
    }

    framebuffer& framebuffer::operator=(framebuffer&& other) noexcept
    {
        _id = other._id;
        _attachments = std::move(other._attachments);
        _readbuffer = other._readbuffer;
        _drawbuffers = std::move(other._drawbuffers);
        other._id = gl_framebuffer_t::zero;
        return *this;
    }

    framebuffer::~framebuffer() noexcept
    {
        if (_id != gl_framebuffer_t::zero)
            glDeleteFramebuffers(1, &_id);
    }

    void framebuffer::set_readbuffer(GLenum attachment) noexcept
    {
        _readbuffer = attachment;
        glNamedFramebufferReadBuffer(_id, _readbuffer);
    }

    void framebuffer::set_drawbuffer(GLenum attachment) noexcept
    {
        _drawbuffers.clear();
        _drawbuffers.emplace_back(attachment);
        glNamedFramebufferDrawBuffer(_id, attachment);
    }

    void framebuffer::set_drawbuffers(std::initializer_list<GLenum> attachments) noexcept
    {
        _drawbuffers.clear();
        _drawbuffers.insert(_drawbuffers.begin(), attachments.begin(), attachments.end());
        glNamedFramebufferDrawBuffers(_id, int(_drawbuffers.size()), _drawbuffers.data());
    }

    attachment& framebuffer::at(GLenum type)
    {
        return _attachments.at(type);
    }

    const attachment& framebuffer::at(GLenum type) const
    {
        return _attachments.at(type);
    }

    attachment& framebuffer::operator[](GLenum type)
    {
        if (_attachments.count(type) == 0)
            _attachments.emplace(type, attachment(*this, type));
        return _attachments.at(type);
    }

    const attachment& framebuffer::operator[](GLenum type) const
    {
        return at(type);
    }

    framebuffer::operator gl_framebuffer_t() const noexcept
    {
        return _id;
    }

    GLenum framebuffer::status() const noexcept
    {
        return glCheckNamedFramebufferStatus(_id, GL_FRAMEBUFFER);
    }

    void framebuffer::status_assert() const noexcept
    {
#if !defined(NDEBUG)
        const GLenum s = status();
        assert(s != GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT && "One of the framebuffer attachment points might be incomplete.");
        assert(s != GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT && "This framebuffer is missing an attachment.");
        assert(s != GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER && "Draw buffer not set correctly.");
        assert(s != GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER && "Read buffer not set correctly.");
        assert(s != GL_FRAMEBUFFER_UNSUPPORTED && "Internal format of any attachment is unsupported.");
        assert(s != GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE && "Detected differently sampled attachments or GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not set on any attachment.");
        assert(s != GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS && "Detected differently layered attachments.");
        assert(s == GL_FRAMEBUFFER_COMPLETE && "Unknown framebuffer error.");
#endif
    }

    attachment& framebuffer::emplace(GLenum type, const std::shared_ptr<texture>& a)
    {
        if (_attachments.count(type) == 0)
            _attachments.emplace(type, attachment(*this, type));
        attachment& att = _attachments.at(type);
        att = a;
        return att;
    }

    attachment& framebuffer::emplace(GLenum type, const std::shared_ptr<renderbuffer>& a)
    {
        if (_attachments.count(type) == 0)
            _attachments.emplace(type, attachment(*this, type));
        attachment& att = _attachments.at(type);
        att = a;
        return att;
    }

    void framebuffer::bind() const noexcept
    {
        if (_id != gl_framebuffer_t::zero) status_assert();
        glBindFramebuffer(GL_FRAMEBUFFER, _id);
    }

    void framebuffer::clear(int color_attachment, const std::initializer_list<float>& color) const
    {
        assert(color.size() >= 4 && "Length check failed!");
        clear(color_attachment, std::data(color));
    }

    void framebuffer::clear(int color_attachment, const float* color) const
    {
        assert(color[0] == color[0] && color[3] == color[3] && "Length access check failed!");
        glClearNamedFramebufferfv(_id, GL_COLOR, color_attachment, color);
    }

    void framebuffer::clear(float depth, int stencil) const noexcept
    {
        glClearNamedFramebufferfi(_id, GL_DEPTH_STENCIL, 0, depth, stencil);
    }

    void framebuffer::blit(const framebuffer& other, GLbitfield mask, GLenum filter) const noexcept
    {
        const attachment& a0 = _attachments.begin()->second;
        return blit(other, 0, 0, a0.width(), a0.height(), mask, filter);
    }

    void framebuffer::blit(const framebuffer& other, int x, int y, int width, int height, GLbitfield mask, GLenum filter) const noexcept
    {
        blit(other, x, y, width, height, x, y, width, height, mask, filter);
    }

    void framebuffer::blit(const framebuffer& other, int src_x, int src_y, int src_width, int src_height, int dst_x, int dst_y, int dst_width, int dst_height, GLbitfield mask, GLenum filter) const noexcept
    {
        glBlitNamedFramebuffer(_id, other, src_x, src_y, src_width, src_height, dst_x, dst_y, dst_width, dst_height, mask, filter);
    }
}
