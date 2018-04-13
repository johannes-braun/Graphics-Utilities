#pragma once

#include <jpu/memory.hpp>
#include <AL/al.h>
#include <glm/glm.hpp>

namespace al
{
    class source : public jpu::ref_count
    {
    public:
        source();
        ~source();

        operator uint32_t() const;

        void play() const;
        void pause() const;
        void stop() const;
        void rewind() const;

        void set(ALenum name, int value) const;
        void set(ALenum name, uint32_t value) const;
        void set(ALenum name, float value) const;
        void set(ALenum name, const glm::vec3& value) const;
        void set(ALenum name, const glm::ivec3& value) const;

    private:
        uint32_t _id;
    };
}
