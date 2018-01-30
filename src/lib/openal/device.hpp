#pragma once

#include <AL/alc.h>
#include <jpu/memory>

namespace al
{
    class default_device : public jpu::ref_count
    {
    public:
        explicit default_device(const char* name = nullptr);
        ~default_device();
        operator ALCdevice*() const;

    private:
        ALCdevice* _device;
    };
    
    class record_device : public jpu::ref_count
    {
    public:
        explicit record_device(const char* name, uint32_t frequency, ALCenum format, int buffer_size);
        ~record_device();
        operator ALCdevice*() const;

        void samples(void* buffer, int samples) const;
        void start() const;
        void end() const;

    private:
        ALCdevice* _device;
    };
}
