#include "formats.hpp"

namespace gfx {
inline namespace v1 {
namespace opengl {
format_info format_from(format format)
{
    switch (format)
    {
    case r8unorm: return {GL_R8, GL_RED, GL_UNSIGNED_BYTE, true, GL_COLOR_BUFFER_BIT};
    case r8snorm: return {GL_R8_SNORM, GL_RED, GL_UNSIGNED_BYTE, true, GL_COLOR_BUFFER_BIT};
    case r8u: return {GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, false, GL_COLOR_BUFFER_BIT};
    case r8i: return {GL_R8I, GL_RED_INTEGER, GL_UNSIGNED_BYTE, false, GL_COLOR_BUFFER_BIT};
    case rg8unorm: return {GL_RG8, GL_RG, GL_UNSIGNED_BYTE, true, GL_COLOR_BUFFER_BIT};
    case rg8snorm: return {GL_RG8_SNORM, GL_RG, GL_UNSIGNED_BYTE, true, GL_COLOR_BUFFER_BIT};
    case rg8u: return {GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE, false, GL_COLOR_BUFFER_BIT};
    case rg8i: return {GL_RG8I, GL_RG_INTEGER, GL_UNSIGNED_BYTE, false, GL_COLOR_BUFFER_BIT};

    case r16unorm: return {GL_R16, GL_RED, GL_UNSIGNED_SHORT, true, GL_COLOR_BUFFER_BIT};
    case r16snorm: return {GL_R16_SNORM, GL_RED, GL_UNSIGNED_SHORT, true, GL_COLOR_BUFFER_BIT};
    case r16u: return {GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT, false, GL_COLOR_BUFFER_BIT};
    case r16i: return {GL_R16I, GL_RED_INTEGER, GL_UNSIGNED_SHORT, false, GL_COLOR_BUFFER_BIT};
    case r16f: return {GL_R16F, GL_RED, GL_HALF_FLOAT, false, GL_COLOR_BUFFER_BIT};
    case r5g6b5unorm: return {GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5_REV, true, GL_COLOR_BUFFER_BIT};
    case rgb8unorm: return {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, true, GL_COLOR_BUFFER_BIT};
    case rgb8snorm: return {GL_RGB8_SNORM, GL_RGB, GL_UNSIGNED_BYTE, true, GL_COLOR_BUFFER_BIT};
    case rgb8u: return {GL_RGB8UI, GL_RGB_INTEGER, GL_UNSIGNED_BYTE, false, GL_COLOR_BUFFER_BIT};
    case rgb8i: return {GL_RGB8I, GL_RGB_INTEGER, GL_UNSIGNED_BYTE, false, GL_COLOR_BUFFER_BIT};
    case rgba8unorm: return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, true, GL_COLOR_BUFFER_BIT};
    case rgba8snorm: return {GL_RGBA8_SNORM, GL_RGBA, GL_UNSIGNED_BYTE, true, GL_COLOR_BUFFER_BIT};
    case rgba8u: return {GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, false, GL_COLOR_BUFFER_BIT};
    case rgba8i: return {GL_RGBA8I, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, false, GL_COLOR_BUFFER_BIT};
    case rg16unorm: return {GL_RG16, GL_RG, GL_UNSIGNED_SHORT, true, GL_COLOR_BUFFER_BIT};
    case rg16snorm: return {GL_RG16_SNORM, GL_RG, GL_UNSIGNED_SHORT, true, GL_COLOR_BUFFER_BIT};
    case rg16u: return {GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT, false, GL_COLOR_BUFFER_BIT};
    case rg16i: return {GL_RG16I, GL_RG_INTEGER, GL_UNSIGNED_SHORT, false, GL_COLOR_BUFFER_BIT};
    case rg16f: return {GL_RG16F, GL_RG, GL_HALF_FLOAT, false, GL_COLOR_BUFFER_BIT};
    case r32u: return {GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, false, GL_COLOR_BUFFER_BIT};
    case r32i: return {GL_R32I, GL_RED_INTEGER, GL_UNSIGNED_INT, false, GL_COLOR_BUFFER_BIT};
    case r32f: return {GL_R32F, GL_RED, GL_FLOAT, false, GL_COLOR_BUFFER_BIT};
    case rgb5a1unorm: return {GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, true, GL_COLOR_BUFFER_BIT};
    case rgb10a2snorm: return {GL_RGB10_A2, GL_RGBA, GL_INT_2_10_10_10_REV, true, GL_COLOR_BUFFER_BIT};
    case rgb10a2unorm: return {GL_RGB10_A2UI, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV, true, GL_COLOR_BUFFER_BIT};
    case r11g11b10f: return {GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, false, GL_COLOR_BUFFER_BIT};
    case rgb9e5: return {GL_RGB9_E5, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV, false, GL_COLOR_BUFFER_BIT};
    case rgb16unorm: return {GL_RGB16, GL_RGB, GL_UNSIGNED_SHORT, true, GL_COLOR_BUFFER_BIT};
    case rgb16snorm: return {GL_RGB16_SNORM, GL_RGB, GL_UNSIGNED_SHORT, true, GL_COLOR_BUFFER_BIT};
    case rgb16u: return {GL_RGB16UI, GL_RGB_INTEGER, GL_UNSIGNED_SHORT, false, GL_COLOR_BUFFER_BIT};
    case rgb16i: return {GL_RGB16I, GL_RGB_INTEGER, GL_UNSIGNED_SHORT, false, GL_COLOR_BUFFER_BIT};
    case rgb16f: return {GL_RGB16F, GL_RGB, GL_HALF_FLOAT, false, GL_COLOR_BUFFER_BIT};
    case rgba16unorm: return {GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT, true, GL_COLOR_BUFFER_BIT};
    case rgba16snorm: return {GL_RGBA16_SNORM, GL_RGBA, GL_UNSIGNED_SHORT, true, GL_COLOR_BUFFER_BIT};
    case rgba16u: return {GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, false, GL_COLOR_BUFFER_BIT};
    case rgba16i: return {GL_RGBA16I, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, false, GL_COLOR_BUFFER_BIT};
    case rgba16f: return {GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, false, GL_COLOR_BUFFER_BIT};
    case rg32u: return {GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT, false, GL_COLOR_BUFFER_BIT};
    case rg32i: return {GL_RG32I, GL_RG_INTEGER, GL_UNSIGNED_INT, false, GL_COLOR_BUFFER_BIT};
    case rg32f: return {GL_RG32F, GL_RG, GL_FLOAT, false, GL_COLOR_BUFFER_BIT};
    case rgb32u: return {GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT, false, GL_COLOR_BUFFER_BIT};
    case rgb32i: return {GL_RGB32I, GL_RGB_INTEGER, GL_UNSIGNED_INT, false, GL_COLOR_BUFFER_BIT};
    case rgb32f: return {GL_RGB32F, GL_RGB, GL_FLOAT, false, GL_COLOR_BUFFER_BIT};
    case rgba32u: return {GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT, false, GL_COLOR_BUFFER_BIT};
    case rgba32i: return {GL_RGBA32I, GL_RGBA_INTEGER, GL_UNSIGNED_INT, false, GL_COLOR_BUFFER_BIT};
    case rgba32f: return {GL_RGBA32F, GL_RGBA, GL_FLOAT, false, GL_COLOR_BUFFER_BIT};

	case bgra8unorm: return {GL_BGRA, GL_BGRA, GL_UNSIGNED_BYTE, true, GL_COLOR_BUFFER_BIT};
	case bgr8unorm: return {GL_BGR, GL_BGR, GL_UNSIGNED_BYTE, true, GL_COLOR_BUFFER_BIT};

    case d16unorm: return {GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, true, GL_DEPTH_BUFFER_BIT};
    case d24unorm: return {GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT_24_8, true, GL_DEPTH_BUFFER_BIT};
    case s8ui: return {GL_STENCIL_INDEX8, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, false, GL_STENCIL_BUFFER_BIT};
    case d32f: return {GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, false, GL_DEPTH_BUFFER_BIT};
    case d24unorms8ui: return {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, true, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT};
    case d32fs8ui: return {GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, true, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT};
    }
    return {};
}
}    // namespace opengl
}    // namespace v1
}    // namespace gfx