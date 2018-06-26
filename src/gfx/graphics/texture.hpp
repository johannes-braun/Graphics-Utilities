#pragma once

#include <gli/gli.hpp>
#include <glm/ext.hpp>

namespace gfx
{
enum class texture_type
{
    tex1d,
    tex2d,
    tex3d
};

namespace detail
{
    class texture_implementation
    {
    public:
        virtual ~texture_implementation()                                           = default;
        virtual void upload(const gli::texture& image, uint32_t layer)                                  = 0;
        virtual void resize(texture_type type, gli::extent3d size, gli::format format, uint32_t levels) = 0;
    };
    std::unique_ptr<texture_implementation> make_texture_implementation();
}

template <texture_type Type> class texture;

constexpr static uint32_t texture_max_mipmaps = 0u;
template <> class texture<texture_type::tex1d>
{
public:
    using dimension_type = uint32_t;
    constexpr static texture_type type = texture_type::tex1d;

    texture(dimension_type size, gli::format format, dimension_type layers = 1, dimension_type mipmaps = texture_max_mipmaps)
            : _implementation(detail::make_texture_implementation())
    {
        _implementation->resize(
                type, {size, layers, 1}, format, mipmaps == texture_max_mipmaps ? gli::levels(gli::extent3d{size, 1, 1}) : mipmaps);
    }

private:
    std::unique_ptr<detail::texture_implementation> _implementation;
};

using texture1d = texture<texture_type::tex1d>;

}
