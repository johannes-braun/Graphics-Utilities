#include "buffer.hpp"
#include <AL/al.h>

namespace al
{
    buffer::buffer()
    {
        alGenBuffers(1, &_id);
    }

    buffer::~buffer()
    {
        alDeleteBuffers(1, &_id);
    }

    buffer::operator unsigned() const
    {
        return _id;
    }

    void buffer::assign(void* data, const size_t size, const ALenum format, const int frequency) const
    {
        alBufferData(_id, format, data, static_cast<ALsizei>(size), frequency);
    }
}
