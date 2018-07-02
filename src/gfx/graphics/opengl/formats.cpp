#include "formats.hpp"

namespace gfx
{
format_info format_from(format format)
{
    switch(format)
    {
    case r8unorm:
        return {GL_R8, GL_RED, GL_UNSIGNED_BYTE, true};
    case r8snorm:
        return {GL_R8_SNORM, GL_RED, GL_UNSIGNED_BYTE, true};
    case r8u:
        return {GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, false};
    case r8i:
        return {GL_R8I, GL_RED_INTEGER, GL_UNSIGNED_BYTE, false};
    case rg8unorm:
        return {GL_RG8, GL_RG, GL_UNSIGNED_BYTE, true};
    case rg8snorm:
        return {GL_RG8_SNORM, GL_RG, GL_UNSIGNED_BYTE, true};
    case rg8u:
        return {GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE, false};
    case rg8i:
        return {GL_RG8I, GL_RG_INTEGER, GL_UNSIGNED_BYTE, false};

    case r16unorm:
        return {GL_R16, GL_RED, GL_UNSIGNED_SHORT, true};
    case r16snorm:
        return {GL_R16_SNORM, GL_RED, GL_UNSIGNED_SHORT, true};
    case r16u:
        return {GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT, false};
    case r16i:
        return {GL_R16I, GL_RED_INTEGER, GL_UNSIGNED_SHORT, false};
    case r16f:
        return {GL_R16F, GL_RED, GL_HALF_FLOAT, false};
    case r5g6b5unorm:
        return {GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5_REV, true};
    case rgb8unorm:
        return {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, true};
    case rgb8snorm:
        return {GL_RGB8_SNORM, GL_RGB, GL_UNSIGNED_BYTE, true};
    case rgb8u:
        return {GL_RGB8UI, GL_RGB_INTEGER, GL_UNSIGNED_BYTE, false};
    case rgb8i:
        return {GL_RGB8I, GL_RGB_INTEGER, GL_UNSIGNED_BYTE, false};
    case rgba8unorm:
        return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, true};
    case rgba8snorm:
        return {GL_RGBA8_SNORM, GL_RGBA, GL_UNSIGNED_BYTE, true};
    case rgba8u:
        return {GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, false};
    case rgba8i:
        return {GL_RGBA8I, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, false};
    case rg16unorm:
        return {GL_RG16, GL_RG, GL_UNSIGNED_SHORT, true};
    case rg16snorm:
        return {GL_RG16_SNORM, GL_RG, GL_UNSIGNED_SHORT, true};
    case rg16u:
        return {GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT, false};
    case rg16i:
        return {GL_RG16I, GL_RG_INTEGER, GL_UNSIGNED_SHORT, false};
    case rg16f:
        return {GL_RG16F, GL_RG, GL_HALF_FLOAT, false};
    case r32u:
        return {GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, false};
    case r32i:
        return {GL_R32I, GL_RED_INTEGER, GL_UNSIGNED_INT, false};
    case r32f:
        return {GL_R32F, GL_RED, GL_FLOAT, false};
    case rgb5a1unorm:
        return {GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, true};
    case rgb10a2snorm:
        return {GL_RGB10_A2, GL_RGBA, GL_INT_2_10_10_10_REV, true};
    case rgb10a2unorm:
        return {GL_RGB10_A2UI, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV, true};
    case r11g11b10f:
        return {GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, false};
    case rgb9e5:
        return {GL_RGB9_E5, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV, false};
    case rgb16unorm:
        return {GL_RGB16, GL_RGB, GL_UNSIGNED_SHORT, true};
    case rgb16snorm:
        return {GL_RGB16_SNORM, GL_RGB, GL_UNSIGNED_SHORT, true};
    case rgb16u:
        return {GL_RGB16UI, GL_RGB_INTEGER, GL_UNSIGNED_SHORT, false};
    case rgb16i:
        return {GL_RGB16I, GL_RGB_INTEGER, GL_UNSIGNED_SHORT, false};
    case rgb16f:
        return {GL_RGB16F, GL_RGB, GL_HALF_FLOAT, false};
    case rgba16unorm:
        return {GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT, true};
    case rgba16snorm:
        return {GL_RGBA16_SNORM, GL_RGBA, GL_UNSIGNED_SHORT, true};
    case rgba16u:
        return {GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, false};
    case rgba16i:
        return {GL_RGBA16I, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, false};
    case rgba16f:
        return {GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, false};
    case rg32u:
        return {GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT, false};
    case rg32i:
        return {GL_RG32I, GL_RG_INTEGER, GL_UNSIGNED_INT, false};
    case rg32f:
        return {GL_RG32F, GL_RG, GL_FLOAT, false};
    case rgb32u:
        return {GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT, false};
    case rgb32i:
        return {GL_RGB32I, GL_RGB_INTEGER, GL_UNSIGNED_INT, false};
    case rgb32f:
        return {GL_RGB32F, GL_RGB, GL_FLOAT, false};
    case rgba32u:
        return {GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT, false};
    case rgba32i:
        return {GL_RGBA32I, GL_RGBA_INTEGER, GL_UNSIGNED_INT, false};
    case rgba32f:
        return {GL_RGBA32F, GL_RGBA, GL_FLOAT, false};
    }
    return {};
}
}
