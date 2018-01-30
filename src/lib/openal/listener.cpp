#include "listener.hpp"
#include "AL/al.h"
#include "glm/gtc/type_ptr.hpp"
#include "jpu/impl/log/log.hpp"

namespace al
{
    listener::listener()
    {
    }

    void listener::set_gain(const float g)
    {
        _gain = g;
        alListenerf(AL_GAIN, g);
    }

    void listener::set_orientation(const glm::quat o)
    {
        _orientation = o;
        glm::vec3 ori[] = {
            o * glm::vec3(0, 0, -1),
            o * glm::vec3(0, 1, 0)
        };
        alListenerfv(AL_ORIENTATION, glm::value_ptr(ori[0]));
    }

    void listener::set_position(const glm::vec3 p)
    {
        _position = p;
        alListener3f(AL_POSITION, p.x, p.y, p.z);
    }

    void listener::set_velocity(const glm::vec3 v)
    {
        _velocity = v;
        alListener3f(AL_VELOCITY, v.x, v.y, v.z);
    }

    float listener::gain() const
    {
        return _gain;
    }

    const glm::quat& listener::orientation() const
    {
        return _orientation;
    }

    const glm::vec3& listener::position() const
    {
        return _position;
    }

    const glm::vec3& listener::velocity() const
    {
        return _velocity;
    }

    void listener::make_current() const
    {
        alListenerf(AL_GAIN, _gain);
        glm::vec3 ori[] = {
            _orientation * glm::vec3(0, 0, -1),
            _orientation * glm::vec3(0, 1, 0)
        };
        alListenerfv(AL_ORIENTATION, glm::value_ptr(ori[0]));
        alListener3f(AL_POSITION, _position.x, _position.y, _position.z);
        alListener3f(AL_VELOCITY, _velocity.x, _velocity.y, _velocity.z);
    }
}
