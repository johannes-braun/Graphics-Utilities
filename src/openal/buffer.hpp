#pragma once

#include <cinttypes>
#include <AL/al.h>

namespace al
{
    class buffer
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
