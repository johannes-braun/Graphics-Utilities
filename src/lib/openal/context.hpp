#pragma once

#include <vector>
#include <jpu/memory>
#include <AL/alc.h>

namespace al
{
    class default_device;

    class context : public jpu::ref_count
    {
    public:
        using attr_list = std::vector<std::pair<int, int>>;
        explicit context(default_device* device, const attr_list& attrs);
        explicit context(default_device* device);
        ~context();

        void make_current() const;

        void suspend() const;
        void process() const;

    private:
        ALCcontext* _context;
    };
}
