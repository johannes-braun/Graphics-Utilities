#pragma once

namespace gfx
{
constexpr projection::projection(float fov, int width, int height, float znear, float zfar, bool neg_y, bool inv_z) noexcept
        : _type(type::perspective)
        , _info(perspective_info{fov, width, height, znear, zfar, neg_y, inv_z})
{
}

constexpr projection::projection(float left, float right, float bottom, float top, float znear, float zfar) noexcept
        : _type(type::orthographic)
        , _info(orthographic_info{left, right, bottom, top, znear, zfar})
{
}

constexpr const projection::orthographic_info& projection::orthographic() const { return std::get<orthographic_info>(_info); }

constexpr projection::orthographic_info& projection::orthographic() { return std::get<orthographic_info>(_info); }

constexpr const projection::perspective_info& projection::perspective() const { return std::get<perspective_info>(_info); }

constexpr projection::perspective_info& projection::perspective() { return std::get<perspective_info>(_info); }
}