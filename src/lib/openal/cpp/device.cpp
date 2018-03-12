#include "../device.hpp"

namespace al
{
    default_device::default_device(const char* name): _device(alcOpenDevice(name))
    {
    }

    default_device::~default_device()
    {
        alcCloseDevice(_device);
    }

    record_device::record_device(const char* name, const uint32_t frequency, const ALCenum format, const int buffer_size)
        : _device(alcCaptureOpenDevice(name, frequency, format, buffer_size))
    {
    }

    record_device::~record_device()
    {
        alcCaptureCloseDevice(_device);
    }

    record_device::operator ALCdevice_struct*() const
    {
        return _device;
    }

    void record_device::samples(void* buffer, const int samples) const
    {
        alcCaptureSamples(_device, buffer, samples);
    }

    void record_device::start() const
    {
        alcCaptureStart(_device);
    }

    void record_device::end() const
    {
        alcCaptureStop(_device);
    }

    default_device::operator ALCdevice*() const
    {
        return _device;
    }
}
