#include "formats.hpp"

namespace gfx
{
size_t format_element_size(format fmt)
{
    switch(fmt)
    {
    case r8unorm:
    case r8snorm:
    case r8u:
    case r8i:
    case s8ui:
        return 1;
    case rg8unorm:
    case rg8snorm:
    case rg8u:
    case rg8i:
    case r16unorm:
    case r16snorm:
    case r16u:
    case r16i:
    case r16f:
    case r5g6b5unorm:
    case d16unorm:
        return 2;
    case rgb8unorm:
    case rgb8snorm:
    case rgb8u:
    case rgb8i:
        return 3;
    case rgba8unorm:
    case rgba8snorm:
    case rgba8u:
    case rgba8i:
    case rg16unorm:
    case rg16snorm:
    case rg16u:
    case rg16i:
    case rg16f:
    case r32u:
    case r32i:
    case r32f:
    case rgb5a1unorm:
    case rgb10a2snorm:
    case rgb10a2unorm:
    case r11g11b10f:
    case rgb9e5:
    case d24unorm:
    case d32f:
    case d24unorms8ui:
        return 4;
    case rgb16unorm:
    case rgb16snorm:
    case rgb16u:
    case rgb16i:
    case rgb16f:
        return 6;
    case rgba16unorm:
    case rgba16snorm:
    case rgba16u:
    case rgba16i:
    case rgba16f:
    case rg32u:
    case rg32i:
    case rg32f:
    case d32fs8ui:
        return 8;
    case rgb32u:
    case rgb32i:
    case rgb32f:
        return 12;
    case rgba32u:
    case rgba32i:
    case rgba32f:
        return 16;
    }
    return 1;
}
}