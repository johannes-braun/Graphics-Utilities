#pragma once

#include <jpu/memory>
#include "pipeline.hpp"

namespace gl
{
    class state : public jpu::ref_count
    {
    public:
        state();
        ~state();

        void capture(basic_primitive primitive) const;

        operator uint32_t() const { return _id; }

    private:
        uint32_t _id;
    };
}
