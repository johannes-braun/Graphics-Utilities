#include <io/window.hpp>
#include <opengl/buffer.hpp>
#include <algorithm>
#include <numeric>
#include <jpu/log.hpp>
#include <res/presets.hpp>
#include <numeric>

int main()
{
    io::window win(io::api::opengl, 800, 600, "Test");

    namespace cb = res::presets::cube;
    gl::buffer<res::vertex> vbo(cb::vertices.begin(), cb::vertices.end());
    gl::buffer<uint32_t> ibo(25, GL_DYNAMIC_STORAGE_BIT);
    std::generate(ibo.begin(), ibo.end(), []()->uint32_t { return uint32_t(rand()); });

    for (const auto idx : ibo)
        log_i << idx;

    std::sort(ibo.begin(), ibo.end());
    log_h << "--------------------------";
    for (const auto idx : ibo)
        log_i << idx;

    system("pause");

    return 0;
}