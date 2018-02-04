#pragma once

#include <glm/glm.hpp>

namespace geo
{
    class scene
    {
    public:
        void set_view(glm::mat4 v);
        void set_projection(glm::mat4 p);
        void set_time(float time);

        const glm::mat4& get_view() const;
        const glm::mat4& get_projection() const;
        const glm::mat4& get_view_projection() const;

        const glm::mat4& get_inv_view() const;
        const glm::mat4& get_inv_projection() const;
        const glm::mat4& get_inv_view_projection() const;

        float get_time() const;
        float get_sin_time() const;

    private:
        glm::mat4 _view;
        glm::mat4 _projection;
        glm::mat4 _view_projection;
        glm::mat4 _inverse_view;
        glm::mat4 _inverse_projection;
        glm::mat4 _inverse_view_projection;

        float _time{0};
        float _sin_time{0};
    };
}
