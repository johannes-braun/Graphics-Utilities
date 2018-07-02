#pragma once

#include "../formats.hpp"
#include <mygl/mygl.hpp>
#include <tuple>

namespace gfx
{
struct format_info
{
    GLenum internal_format;
    GLenum external_format;
    GLenum external_type;
    bool   normalized;
};

format_info format_from(format format);
}