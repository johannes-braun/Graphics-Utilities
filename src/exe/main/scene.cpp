#include "scene.hpp"

namespace geo
{
    void scene::set_view(glm::mat4 v)
    {
        _view = v;
        _view_projection =  _projection * _view;
        _inverse_view = glm::inverse(_view);
        _inverse_view_projection = _inverse_view * _inverse_projection;
    }

    void scene::set_projection(glm::mat4 p)
    {
        _projection = p;
        _view_projection = _projection * _view;
        _inverse_projection = glm::inverse(_projection);
        _inverse_view_projection = _inverse_view * _inverse_projection;
    }

    void scene::set_time(float time)
    {
        _time = time;
        _sin_time = glm::sin(time);
    }

    const glm::mat4& scene::get_view() const
    {
        return _view;
    }

    const glm::mat4& scene::get_projection() const
    {
        return _projection;
    }

    const glm::mat4& scene::get_view_projection() const
    {
        return _view_projection;
    }

    const glm::mat4& scene::get_inv_view() const
    {
        return _inverse_view;
    }

    const glm::mat4& scene::get_inv_projection() const
    {
        return _inverse_projection;
    }

    const glm::mat4& scene::get_inv_view_projection() const
    {
        return _inverse_view_projection;
    }

    float scene::get_time() const
    {
        return _time;
    }

    float scene::get_sin_time() const
    {
        return _sin_time;
    }
}
