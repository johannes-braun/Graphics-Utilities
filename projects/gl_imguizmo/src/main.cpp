#define _SCL_SECURE_NO_WARNINGS

#include "buffer.hpp"
#include "context.hpp"
#include "swapchain.hpp"
#include <glm/glm.hpp>

#include <mygl/mygl.hpp>

#include "context_vulkan.hpp"

void dbg(GLenum source, GLenum type, unsigned int id, GLenum severity, int length,
         const char* message, const void* userParam)
{
    gfx::cloge << message;
}

int main()
{
    auto ctx = gfx::context::create(gfx::gapi::vulkan, "Context", 1280, 720);

    // Buffers etc for vulkan
    ctx->make_current();
    gfx::host_buffer<int>   input{3, 4, 581, 58189382, 38, 1839, 1289347};
    gfx::device_buffer<int> device_target(input.size());
    gfx::device_buffer<int> device_target2(input.size());
    gfx::host_buffer<int>   readout(input.size());
    gfx::swapchain swapchain;

    // Run Vulkan
    double vfps    = 0;
    int    vframes = 0;
    ctx->do_on_update([&](double delta) {
        vfps += delta;
        ++vframes;
        if(vfps >= 1.0)
        {
            gfx::clogw("FPS") << vframes / vfps;
            vfps    = 0;
            vframes = 0;
        }
        device_target << input;
        device_target2 << device_target;
        device_target >> readout;

        if(glFinish)
            glFinish();
        else
        {
            static_cast<gfx::vulkan::context_implementation*>(
                    std::any_cast<gfx::detail::context_implementation*>(ctx->implementation()))
                    ->device()
                    .waitIdle();
        }

    });

    while(true)
    {
        ctx->run();
    }
}
