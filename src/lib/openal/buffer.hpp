#pragma once

#include <cinttypes>
#include <AL/al.h>
#include <jpu/memory>

namespace al
{
    class buffer : public jpu::ref_count
    {
    public:
        buffer();
        ~buffer();
        operator unsigned() const;

        void assign(void* data, size_t size, ALenum format, int frequency) const;

    private:
        uint32_t _id;
    };
}
