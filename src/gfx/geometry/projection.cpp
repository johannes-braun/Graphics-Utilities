#include <gfx/geometry.hpp>

namespace gfx
{
glm::mat4 projection::matrix() const noexcept
{
    switch(_type)
    {
    case type::perspective:
    {
        const perspective_info& info      = std::get<perspective_info>(_info);
        const glm::mat4         inv_z_mat = transpose(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 1, 0, 0, 0, 1));
        const glm::mat4         neg_y_mat = transpose(glm::mat4(1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));

        return (info.inverse_z ? inv_z_mat : glm::mat4(1.f)) * (info.negative_y ? neg_y_mat : glm::mat4(1.f)) *
               glm::perspectiveFov(info.field_of_view,
                                   float(std::max(info.screen_width, info.screen_height)),
                                   float(std::min(info.screen_width, info.screen_height)),
                                   info.clip_near,
                                   info.clip_far);
    }
    case type::orthographic:
    {
        const orthographic_info& info = std::get<orthographic_info>(_info);
        return info.clip_near == -1.f || info.clip_far == -1.f
                       ? glm::ortho(info.left, info.right, info.bottom, info.top)
                       : glm::ortho(info.left, info.right, info.bottom, info.top, info.clip_near, info.clip_far);
    }
    default:
        return glm::mat4(1.f);
    }
}

projection::operator glm::mat4() const noexcept { return matrix(); }
}