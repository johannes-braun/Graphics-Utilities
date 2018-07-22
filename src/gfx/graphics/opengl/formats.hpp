#pragma once

#include "../formats.hpp"
#include <mygl/mygl.hpp>
#include <tuple>

namespace gfx {
inline namespace v1 {
namespace opengl {
struct format_info
{
    GLenum internal_format;
    GLenum external_format;
    GLenum external_type;
    bool   normalized;
	GLenum buffer_bit;
};

format_info format_from(format format);
}    // namespace opengl
}    // namespace v1
}    // namespace gfx