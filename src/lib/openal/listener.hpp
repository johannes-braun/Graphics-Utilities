#pragma once
#include "glm/gtx/quaternion.hpp"
#include <jpu/memory>

namespace al
{
    class listener : public jpu::ref_count
    {
    public:
        listener();
        ~listener() = default;

        void set_gain(float g);
        void set_orientation(glm::quat o);
        void set_position(glm::vec3 p);
        void set_velocity(glm::vec3 v);

        float gain() const;
        const glm::quat& orientation() const;
        const glm::vec3& position() const;
        const glm::vec3& velocity() const;

        void make_current() const;

    private:
        float _gain{1.f};
        glm::quat _orientation{1, 0, 0, 0};
        glm::vec3 _position{0, 0, 0};
        glm::vec3 _velocity{0, 0, 0};
    };
}
