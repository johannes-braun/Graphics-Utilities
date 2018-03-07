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

        operator gl_state_nv_t() const { return _id; }

    private:
        gl_state_nv_t _id;
    };
}
