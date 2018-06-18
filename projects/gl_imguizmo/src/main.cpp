#define _SCL_SECURE_NO_WARNINGS

#include "context.hpp"
#include "swapchain.hpp"

#include <glm/glm.hpp>
#include "buffer.hpp"

int main()
{
    auto context = gfx::context::create(gfx::gapi::opengl, "Test", 1280, 720);
    context->make_current();

    gfx::swapchain swapchain;

    std::array<int32_t, 35> arr;
    std::iota(arr.begin(), arr.end(), 0);
    std::vector<glm::vec4> vectors{8, glm::vec4(8, 0, 0, 0)};

    gfx::host_buffer<float>   float_buffer{12.f, 11.f, 239.f, 12390.f, 1.f, 19302.f};
    gfx::host_buffer<int32_t> int_buffer;
    int_buffer.resize(1 << 20);
    std::iota(int_buffer.begin(), int_buffer.end(), 0);
    gfx::host_buffer<glm::vec4> vec4_buffer = vectors;
    gfx::host_buffer<uint32_t>  empty_vector;

    gfx::device_buffer<int32_t> dint_buffer1(int_buffer.size());
    gfx::device_buffer<int32_t> dint_buffer2(int_buffer.size());
    gfx::host_buffer<int32_t>  dint_target(int_buffer.size());

    int i = 0;
    while(true)
    {
        uint32_t image = swapchain.current_image();

        dint_buffer1 << int_buffer;
        dint_buffer2 << dint_buffer1;
        dint_buffer2.update({i++}, 16);
        dint_buffer2 >> dint_target;

        swapchain.present();
        glfwPollEvents();
    }
}
