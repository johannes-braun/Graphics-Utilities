#include "source.hpp"

namespace al
{
    source::source()
    {
        alGenSources(1, &_id);
    }

    source::~source()
    {
        alDeleteSources(1, &_id);
    }

    source::operator unsigned() const
    {
        return _id;
    }

    void source::play() const
    {
        alSourcePlay(_id);
    }

    void source::pause() const
    {
        alSourcePause(_id);
    }

    void source::stop() const
    {
        alSourceStop(_id);
    }

    void source::rewind() const
    {
        alSourceRewind(_id);
    }

    void source::set(const ALenum name, const int value) const
    {
        alSourcei(_id, name, value);
    }

    void source::set(const ALenum name, const uint32_t value) const
    {
        alSourcei(_id, name, static_cast<ALint>(value));
    }

    void source::set(const ALenum name, const float value) const
    {
        alSourcef(_id, name, value);
    }

    void source::set(const ALenum name, const glm::ivec3& value) const
    {
        alSource3i(_id, name, value.x, value.y, value.z);
    }

    void source::set(const ALenum name, const glm::vec3& value) const
    {
        alSource3f(_id, name, value.x, value.y, value.z);
    }
}
