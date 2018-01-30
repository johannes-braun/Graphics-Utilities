#include "context.hpp"
#include "device.hpp"

namespace al
{
    context::context(default_device* device, const attr_list& attrs)
        : _context(alcCreateContext(*device, &(attrs[0].first)))
    {
    }

    context::context(default_device* device)
        : _context(alcCreateContext(*device, nullptr))
    {
    }

    context::~context()
    {
        alcDestroyContext(_context);
    }

    void context::make_current() const
    {
        alcMakeContextCurrent(_context);
    }

    void context::suspend() const
    {
        alcSuspendContext(_context);
    }

    void context::process() const
    {
        alcProcessContext(_context);
    }
}
