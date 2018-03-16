#include <io/window.hpp>
#include <opengl/buffer.hpp>
#include <algorithm>
#include <numeric>
#include <jpu/log.hpp>
#include <res/presets.hpp>
#include "bvh.hpp"

int main()
{
    io::window win(io::api::opengl, 800, 600, "Test");

    namespace cb = res::presets::cube;
    gl::buffer<res::vertex> vbo(cb::vertices.begin(), cb::vertices.end());
    gl::buffer<uint32_t> ibo(cb::indices.begin(), cb::indices.end(), GL_DYNAMIC_STORAGE_BIT);

    for (const auto idx : ibo)
        log_i << idx;

    std::sort(ibo.begin(), ibo.end());
    std::iter_swap(ibo.begin(), ibo.begin() + 4);
    log_h << "--------------------------";
    for (const auto idx : ibo)
        log_i << idx;

    system("pause");

    return 0;
}