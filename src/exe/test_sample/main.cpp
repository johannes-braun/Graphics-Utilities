#include <atomic>
#include <array>
#include <jpu/log>
#include <omp.h>
#include <jpu/memory>
#include <io/window.hpp>
#include <jpu/data>

constexpr int Logn = 19;
std::vector<uint32_t> data1(1 << Logn);
std::vector<uint32_t> data2(1 << Logn);

void bitonic(int logn, uint32_t* a, size_t len)
{
    for (int p = 0; p < logn; ++p)
    {
        for (int q = 0; q <= p; ++q)
        {
            const int d = 1 << (p - q);
            const int trg = len >> 1;

#pragma omp parallel for schedule(static)
            for (int id = 0; id < len / 2; ++id)
            {

                const int gid = (id + d * (id / d));
                const bool up = ((gid >> p) & 2) == 0;

                if ((a[gid] > a[gid | d]) == up)
                    std::swap(a[gid], a[gid | d]);
            }
        }
    }
}

jpu::ref_ptr<io::window> main_window;
jpu::named_vector<std::string, jpu::ref_ptr<gl::graphics_pipeline>> graphics_pipelines;

int main(int argc, const char** argv)
{
    gl::setup_shader_paths("../shaders");

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = jpu::make_ref<io::window>(io::api::opengl, 1280, 720, "My Window");
    main_window->load_icon("../res/ui/logo.png");
    glfwSwapInterval(0);
    // glfwSwapInterval(1);
    glfwSetKeyCallback(*main_window, [](GLFWwindow*, int key, int, int action, int mods) {
        if (main_window->gui()->key_action(key, action, mods))
            return;

        if (action == GLFW_PRESS && key == GLFW_KEY_P)
            for (auto&& p : graphics_pipelines)
                p->reload_stages();
    });

    glfwSetScrollCallback(*main_window, [](GLFWwindow*, double x, double y) {
        main_window->gui()->scrolled(y);
    });

    glfwSetCharCallback(*main_window, [](GLFWwindow*, uint32_t ch) {
        if (main_window->gui()->char_input(static_cast<wchar_t>(ch)))
            return;
    });

    glfwSetMouseButtonCallback(*main_window, [](GLFWwindow*, int btn, int action, int mods) {
        if (main_window->gui()->mouse_button_action(btn, action, mods))
            return;
    });

    std::generate(data1.begin(), data1.end(), []() { return rand(); });
    data2 = data1;
    auto buf = jpu::make_ref<gl::buffer>(data1, gl::buffer_flag_bits::map_dynamic_persistent);
    buf->bind(0, GL_SHADER_STORAGE_BUFFER);

    auto cmp = jpu::make_ref<gl::compute_pipeline>(
        jpu::make_ref<gl::shader>(gl::shader_root_path / "sort/bitonic.comp"));
    glFinish();

    auto t = std::chrono::system_clock::now();
    const int trg = data1.size() >> 1;
    cmp->bind();
    auto&& stage = cmp->stage(gl::shader_type::compute);
    auto puni = stage->get_uniform<int>("p");
    auto quni = stage->get_uniform<int>("q");
    auto duni = stage->get_uniform<int>("d");
    for (int p = 0; p < Logn; ++p)
    {
        puni = p;
        for (int q = 0; q <= p; ++q)
        {
            const int d = 1 << (p - q);
            duni = d;
            quni = q;
            cmp->dispatch(trg);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
    }
    glFinish();
    log_i << (std::chrono::system_clock::now() - t).count();

    t = std::chrono::system_clock::now();
    std::sort(data2.begin(), data2.end());
    log_i << (std::chrono::system_clock::now() - t).count();


    std::vector<uint32_t> out(buf->data_as<uint32_t>(), buf->data_as<uint32_t>() + buf->size() / sizeof(uint32_t));
    system("pause");
    return 0;
}
