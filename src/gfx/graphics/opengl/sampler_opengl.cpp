#include "sampler_opengl.hpp"
#include <glm/ext.hpp>
#include <mygl/mygl.hpp>

namespace gfx::opengl
{
sampler_implementation::sampler_implementation()
{
    glCreateSamplers(1, &_handle);
    glSamplerParameteri(_handle, GL_TEXTURE_CUBE_MAP_SEAMLESS, true);
}

sampler_implementation::~sampler_implementation()
{
    if (glIsSampler(_handle)) glDeleteSamplers(1, &_handle);
}

std::any sampler_implementation::api_handle()
{
    return _handle;
}

void sampler_implementation::set_filter(filter_mode mode, filter filter)
{
    if (mode == filter_mode::mipmap) {
        _mipmap_filter = filter;
    }
    else
    {
        if (mode == filter_mode::min)
            _min_filter = filter;
        else
            glSamplerParameteri(_handle, GL_TEXTURE_MAG_FILTER, filter == filter::nearest ? GL_NEAREST : GL_LINEAR);
    }

    glSamplerParameteri(_handle, GL_TEXTURE_MIN_FILTER,
                        _mipmap_filter == filter::nearest
                            ? (_min_filter == filter::nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_NEAREST)
                            : (_min_filter == filter::nearest ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR));
}

void sampler_implementation::set_wrap(wrap w, wrap_mode mode)
{
    const auto wm = [&]() {
        switch (mode)
        {
        case wrap_mode::none: return GL_NONE;
        case wrap_mode::repeat: return GL_REPEAT;
        case wrap_mode::mirror_repeat: return GL_MIRRORED_REPEAT;
        case wrap_mode::clamp_to_edge: return GL_CLAMP_TO_EDGE;
        case wrap_mode::clamp_to_border: return GL_CLAMP_TO_BORDER;
        case wrap_mode::mirror_clamp_to_edge: return GL_MIRROR_CLAMP_TO_EDGE;
        default: return GLenum(0);
        }
    }();

    switch (w)
    {
    case wrap::u: glSamplerParameteri(_handle, GL_TEXTURE_WRAP_S, wm); break;
    case wrap::v: glSamplerParameteri(_handle, GL_TEXTURE_WRAP_T, wm); break;
    case wrap::w: glSamplerParameteri(_handle, GL_TEXTURE_WRAP_R, wm); break;
    default: break;
    }
}

void sampler_implementation::set_border(border_color color)
{
    switch (color)
    {
    case border_color::float_transparent_black:
    {
        glm::vec4 col(0, 0, 0, 0);
        glSamplerParameterfv(_handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(col));
    }
    break;
    case border_color::int_transparent_black:
    {
        glm::ivec4 col(0, 0, 0, 0);
        glSamplerParameteriv(_handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(col));
    }
    break;
    case border_color::float_opaque_black:
    {
        glm::vec4 col(0, 0, 0, 1);
        glSamplerParameterfv(_handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(col));
    }
    break;
    case border_color::int_opaque_black:
    {
        glm::ivec4 col(0, 0, 0, std::numeric_limits<int32_t>::max());
        glSamplerParameteriv(_handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(col));
    }
    break;
    case border_color::float_opaque_white:
    {
        glm::vec4 col(1);
        glSamplerParameterfv(_handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(col));
    }
    break;
    case border_color::int_opaque_white:
    {
        glm::ivec4 col(std::numeric_limits<int32_t>::max());
        glSamplerParameteriv(_handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(col));
    }
    break;
    default: break;
    }
}

void sampler_implementation::set_lod(lod mode, float value)
{
    switch (mode)
    {
    case lod::bias: glSamplerParameterf(_handle, GL_TEXTURE_LOD_BIAS, value); break;
    case lod::min: glSamplerParameterf(_handle, GL_TEXTURE_MIN_LOD, value); break;
    case lod::max: glSamplerParameterf(_handle, GL_TEXTURE_MAX_LOD, value); break;
    default: break;
    }
}

void sampler_implementation::set_anisotropy(bool enable, float value)
{
    glSamplerParameterf(_handle, GL_TEXTURE_MAX_ANISOTROPY, enable ? value : 0.f);
}

void sampler_implementation::set_compare(bool enable, compare_op op)
{
    glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_MODE, enable ? GL_NONE : GL_COMPARE_REF_TO_TEXTURE);
    switch (op)
    {
    case compare_op::never: glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_FUNC, GL_NEVER); break;
    case compare_op::less: glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_FUNC, GL_LESS); break;
    case compare_op::equal: glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_FUNC, GL_EQUAL); break;
    case compare_op::less_or_equal: glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL); break;
    case compare_op::greater: glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_FUNC, GL_GREATER); break;
    case compare_op::not_equal: glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_FUNC, GL_NOTEQUAL); break;
    case compare_op::greater_or_equal: glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL); break;
    case compare_op::always: glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_FUNC, GL_ALWAYS); break;
    default: break;
    }
}
}    // namespace gfx::opengl
