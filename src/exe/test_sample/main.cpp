#include <atomic>
#include <array>
#include <jpu/log>
#include <jpu/memory>
#include <io/window.hpp>
#include <jpu/data>
#include <stack>
#include "opengl/query.hpp"
#include <numeric>
#include <res/image.hpp>

constexpr int logn = 18;
std::vector<float> data1(1 << logn);
std::vector<float> data2(1 << logn);

void bitonic(const int logn, uint32_t* a, const size_t len)
{
    for (int p = 0; p < logn; ++p)
    {
        for (int q = 0; q <= p; ++q)
        {
            const int d = 1 << (p - q);

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

int main()
{
    gl::setup_shader_paths("../shaders");

   // glfwWindowHint(GLFW_SAMPLES, 8);
   // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = jpu::make_ref<io::window>(io::api::opengl, 1280, 720, "My Window");

    res::image icon = load_image("../res/ui/logo.png", res::image_type::unsigned_byte, res::image_components::rgb_alpha);
    res::image cursor = load_image("../res/cursor.png", res::image_type::unsigned_byte, res::image_components::rgb_alpha);

    main_window->set_icon(icon.width, icon.height, icon.data.get());
    main_window->set_cursor(new io::cursor(cursor.width, cursor.height, cursor.data.get(), 0, 0));
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
    data2 = data1;


    std::generate(data1.begin(), data1.end(), []() { return static_cast<float>(rand()); });
    auto buf = jpu::make_ref<gl::buffer>(data1);
    buf->bind(0, GL_SHADER_STORAGE_BUFFER);
    glFinish();

    auto rdx_bckt = jpu::make_ref<gl::compute_pipeline>(
        jpu::make_ref<gl::shader>(gl::shader_root_path / "sort/radix_bucket.comp"));
    auto cmp = jpu::make_ref<gl::compute_pipeline>(
        jpu::make_ref<gl::shader>(gl::shader_root_path / "sort/bitonic.comp"));
    log_i << data1.size();
    auto&& stage = cmp->stage(gl::shader_type::compute);

    gl::query query(GL_TIME_ELAPSED);

    query.begin();
    rdx_bckt->bind();
    rdx_bckt->dispatch(data1.size());
    query.end();
    log_i << query.get_uint64();

    query.begin();
    rdx_bckt->bind();
    rdx_bckt->dispatch(data1.size());
    query.end();
    log_i << query.get_uint64();

    query.begin();
    rdx_bckt->bind();
    rdx_bckt->dispatch(data1.size());
    query.end();
    log_i << query.get_uint64();
    
    const auto run = [&]() {
        std::generate(data1.begin(), data1.end(), []() { return rand(); });
        buf = jpu::make_ref<gl::buffer>(data1);
        buf->bind(0, GL_SHADER_STORAGE_BUFFER);
        glFinish();
        const int trg = data1.size() >> 1;
        auto puni = stage->get_uniform<int>("p");
        auto quni = stage->get_uniform<int>("q");
        auto duni = stage->get_uniform<int>("d");

        cmp->bind();
        query.begin();
        for (int p = 0; p < logn; ++p)
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
        query.end();
        uint64_t time = query.get_uint64();
        log_i << time / 1000000.f;
    };

    run();
    run();
    run();
    run();

    run();
    run();
    run();
    run();

    run();
    run();
    run();
    run();


    const auto t = std::chrono::steady_clock::now();
    // bitonic(Logn, reinterpret_cast<uint32_t*>(data2.data()), data2.size());
    std::sort(data2.begin(), data2.end());
    log_i << (std::chrono::steady_clock::now() - t).count();


   // std::vector<uint32_t> out(buf->data_as<uint32_t>(), buf->data_as<uint32_t>() + buf->size() / sizeof(uint32_t));
    log_i << "";
    system("pause");
    return 0;
}
