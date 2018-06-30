#include "device_image_opengl.hpp"

namespace gfx::opengl
{
std::tuple<GLenum, GLenum, GLenum> format_from(img_format format)
{
    switch(format)
    {
    case r8unorm:
        return {GL_R8, GL_RED, GL_UNSIGNED_BYTE};
    case r8snorm:
        return {GL_R8_SNORM, GL_RED, GL_UNSIGNED_BYTE};
    case r8u:
        return {GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE};
    case r8i:
        return {GL_R8I, GL_RED_INTEGER, GL_UNSIGNED_BYTE};
    case rg8unorm:
        return {GL_RG8, GL_RG, GL_UNSIGNED_BYTE};
    case rg8snorm:
        return {GL_RG8_SNORM, GL_RG, GL_UNSIGNED_BYTE};
    case rg8u:
        return {GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE};
    case rg8i:
        return {GL_RG8I, GL_RG_INTEGER, GL_UNSIGNED_BYTE};

    case r16unorm:
        return {GL_R16, GL_RED, GL_UNSIGNED_SHORT};
    case r16snorm:
        return {GL_R16_SNORM, GL_RED, GL_UNSIGNED_SHORT};
    case r16u:
        return {GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT};
    case r16i:
        return {GL_R16I, GL_RED_INTEGER, GL_UNSIGNED_SHORT};
    case r16f:
        return {GL_R16F, GL_RED, GL_HALF_FLOAT};
    case r5g6b5unorm:
        return {GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5_REV};
    case rgb8unorm:
        return {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE};
    case rgb8snorm:
        return {GL_RGB8_SNORM, GL_RGB, GL_UNSIGNED_BYTE};
    case rgb8u:
        return {GL_RGB8UI, GL_RGB_INTEGER, GL_UNSIGNED_BYTE};
    case rgb8i:
        return {GL_RGB8I, GL_RGB_INTEGER, GL_UNSIGNED_BYTE};
    case rgba8unorm:
        return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE};
    case rgba8snorm:
        return {GL_RGBA8_SNORM, GL_RGBA, GL_UNSIGNED_BYTE};
    case rgba8u:
        return {GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE};
    case rgba8i:
        return {GL_RGBA8I, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE};
    case rg16unorm:
        return {GL_RG16, GL_RG, GL_UNSIGNED_SHORT};
    case rg16snorm:
        return {GL_RG16_SNORM, GL_RG, GL_UNSIGNED_SHORT};
    case rg16u:
        return {GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT};
    case rg16i:
        return {GL_RG16I, GL_RG_INTEGER, GL_UNSIGNED_SHORT};
    case rg16f:
        return {GL_RG16F, GL_RG, GL_HALF_FLOAT};
    case r32u:
        return {GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT};
    case r32i:
        return {GL_R32I, GL_RED_INTEGER, GL_UNSIGNED_INT};
    case r32f:
        return {GL_R32F, GL_RED, GL_FLOAT};
    case rgb5a1unorm:
        return {GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV};
    case rgb10a2snorm:
        return {GL_RGB10_A2, GL_RGBA, GL_INT_2_10_10_10_REV};
    case rgb10a2unorm:
        return {GL_RGB10_A2UI, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV};
    case r11g11b10f:
        return {GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV};
    case rgb9e5:
        return {GL_RGB9_E5, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV};
    case rgb16unorm:
        return {GL_RGB16, GL_RGB, GL_UNSIGNED_SHORT};
    case rgb16snorm:
        return {GL_RGB16_SNORM, GL_RGB, GL_UNSIGNED_SHORT};
    case rgb16u:
        return {GL_RGB16UI, GL_RGB_INTEGER, GL_UNSIGNED_SHORT};
    case rgb16i:
        return {GL_RGB16I, GL_RGB_INTEGER, GL_UNSIGNED_SHORT};
    case rgb16f:
        return {GL_RGB16F, GL_RGB, GL_HALF_FLOAT};
    case rgba16unorm:
        return {GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT};
    case rgba16snorm:
        return {GL_RGBA16_SNORM, GL_RGBA, GL_UNSIGNED_SHORT};
    case rgba16u:
        return {GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT};
    case rgba16i:
        return {GL_RGBA16I, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT};
    case rgba16f:
        return {GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT};
    case rg32u:
        return {GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT};
    case rg32i:
        return {GL_RG32I, GL_RG_INTEGER, GL_UNSIGNED_INT};
    case rg32f:
        return {GL_RG32F, GL_RG, GL_FLOAT};
    case rgb32u:
        return {GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT};
    case rgb32i:
        return {GL_RGB32I, GL_RGB_INTEGER, GL_UNSIGNED_INT};
    case rgb32f:
        return {GL_RGB32F, GL_RGB, GL_FLOAT};
    case rgba32u:
        return {GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT};
    case rgba32i:
        return {GL_RGBA32I, GL_RGBA_INTEGER, GL_UNSIGNED_INT};
    case rgba32f:
        return {GL_RGBA32F, GL_RGBA, GL_FLOAT};
    }
    return {};
}

void device_image_implementation::initialize(uint32_t layer_dimensions, img_format format, const extent& size, uint32_t levels)
{
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    auto x           = format_from(format);
    _internal_format = std::get<0>(x);
    _external_format = std::get<1>(x);
    _external_type   = std::get<2>(x);
    _extent          = size;
    _levels          = levels;
    switch(layer_dimensions)
    {
    case 1:
    {
        _type = GL_TEXTURE_1D_ARRAY;
        glCreateTextures(_type, 1, &_handle);
        glTextureStorage2D(_handle, _levels, _internal_format, _extent.width, _extent.height);
    }
    break;
    case 2:
    {
        _type = GL_TEXTURE_2D_ARRAY;
        glCreateTextures(_type, 1, &_handle);
        glTextureStorage3D(_handle, _levels, _internal_format, _extent.width, _extent.height, _extent.depth);
    }
    break;
    case 3:
    {
        _type = GL_TEXTURE_3D;
        glCreateTextures(_type, 1, &_handle);
        glTextureStorage3D(_handle, _levels, _internal_format, _extent.width, _extent.height, _extent.depth);
    }
    break;
    }
}
void device_image_implementation::fill_from(const host_image& image, uint32_t level, uint32_t layer)
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, std::any_cast<mygl::buffer>(image.storage().api_handle()));
    switch(_type)
    {
    case GL_TEXTURE_3D:
        glTextureSubImage3D(
                _handle, level, 0, 0, 0, _extent.width, _extent.height, _extent.depth, _external_format, _external_type, nullptr);
    case GL_TEXTURE_2D_ARRAY:
        glTextureSubImage3D(_handle, level, 0, 0, layer, _extent.width, _extent.height, 1, _external_format, _external_type, nullptr);
    case GL_TEXTURE_1D_ARRAY:
        glTextureSubImage2D(_handle, level, 0, layer, _extent.width, 1, _external_format, _external_type, nullptr);
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mygl::buffer::zero);
    glFinish();
}
}