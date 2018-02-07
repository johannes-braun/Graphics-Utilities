#include <atomic>
#include <array>
#include <jpu/log>
#include <omp.h>
#include <jpu/memory>
#include <io/window.hpp>
#include <jpu/data>
#include <stack>

constexpr int Logn = 13;
std::vector<float> data1(1 << Logn);
std::vector<float> data2(1 << Logn);

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

class GPU_Clock
{
public:
    /**
    * \brief Retrieves/Creates the Singleton instance.
    * \return the instance.
    */
    static GPU_Clock& instance();
    ~GPU_Clock();

    GPU_Clock(GPU_Clock&&) = delete;
    GPU_Clock& operator=(GPU_Clock&& other) = delete;
    // automatically no copy operations now

    /**
    * \brief Start the Clock.
    */
    void start();

    /**
    * \brief End the Clock.
    * \return The time measured in Nanoseconds.
    */
    GLuint64 end();

private:
    /**
    * \brief Wrapper-Class for GPU-Queries with GL_TIME_ELAPSED.
    * Cannot be nested, thus this class is private because GPU_Clock works around that problem.
    */
    class TimerQuery
    {
    public:

        TimerQuery() { glGenQueries(1, &m_id); };
        explicit TimerQuery(GLuint id) : m_id(id) {};
        ~TimerQuery() { glDeleteQueries(1, &m_id); };

        TimerQuery(TimerQuery&&) = delete;
        TimerQuery& operator=(TimerQuery&& other) = delete;
        // automatically no copy operations now

        /**
        * \brief Begins the Query GL_TIME_ELAPSED.
        */
        void issue() const { glBeginQuery(GL_TIME_ELAPSED, m_id); };

        /**
        * \brief Ends the Query GL_TIME_ELAPSED, waits for the result and returns it.
        * \return The elapsed time since issue(), measured in Nanoseconds.
        */
        GLuint64 get() const
        {
            glEndQuery(GL_TIME_ELAPSED);
            GLuint64 result;
            glGetQueryObjectui64v(m_id, GL_QUERY_RESULT, &result);
            return result;
        };

    private:
        GLuint m_id;
    };

    GPU_Clock();

    TimerQuery m_query;
    int m_current_index = 0;
    std::vector<GLuint64> m_times;
    std::stack<int> m_start_indices;
};

GPU_Clock::GPU_Clock()
{
}

GPU_Clock& GPU_Clock::instance()
{
    static GPU_Clock _instance;
    return _instance;
}

void GPU_Clock::start()
{
    if (!m_start_indices.empty())
    {
        m_times.push_back(m_query.get());
        m_current_index++;
    }
    m_start_indices.push(m_current_index);
    m_query.issue();
}

GLuint64 GPU_Clock::end()
{
    m_current_index++;
    m_times.push_back(m_query.get());
    int index = m_start_indices.top();
    m_start_indices.pop();
    GLuint64 elapsed_time = 0;
    elapsed_time = std::accumulate(m_times.begin() + index, m_times.begin() + m_current_index, elapsed_time);
    if (m_start_indices.empty())
    {
        m_current_index = 0;
        m_times.clear();
    }
    else { m_query.issue(); }
    return elapsed_time;
}

GPU_Clock::~GPU_Clock() {}


jpu::ref_ptr<io::window> main_window;
jpu::named_vector<std::string, jpu::ref_ptr<gl::graphics_pipeline>> graphics_pipelines;

int main(int argc, const char** argv)
{
    gl::setup_shader_paths("../shaders");

   // glfwWindowHint(GLFW_SAMPLES, 8);
   // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
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
    data2 = data1;

    std::generate(data1.begin(), data1.end(), []() { return rand(); });
    auto buf = jpu::make_ref<gl::buffer>(data1);
    buf->bind(0, GL_SHADER_STORAGE_BUFFER);






    auto rdx_bckt = jpu::make_ref<gl::compute_pipeline>(
        jpu::make_ref<gl::shader>(gl::shader_root_path / "sort/radix_bucket.comp"));

    auto cmp = jpu::make_ref<gl::compute_pipeline>(
        jpu::make_ref<gl::shader>(gl::shader_root_path / "sort/bitonic.comp"));
    log_i << data1.size();
    cmp->bind();
    auto&& stage = cmp->stage(gl::shader_type::compute);
    auto puni = stage->get_uniform<int>("p");
    auto quni = stage->get_uniform<int>("q");
    auto duni = stage->get_uniform<int>("d");
    auto count = stage->get_uniform<int>("count");

    const int trg = data1.size() >> 1;
    GPU_Clock::instance().start();
    int i = 0;
    for (int p = 0; p < Logn; ++p)
    {
        puni = p;
        for (int q = 0; q <= p; ++q)
        {
            const int d = 1 << (p - q);
            duni = d;
            quni = q;
            count = trg;
            cmp->dispatch(trg);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            ++i;
        }
    }
    log_i << GPU_Clock::instance().end();

    log_i << "I is " << i;

    auto t = std::chrono::steady_clock::now();
    // bitonic(Logn, reinterpret_cast<uint32_t*>(data2.data()), data2.size());
    std::sort(data2.begin(), data2.end());
    log_i << (std::chrono::steady_clock::now() - t).count();


   // std::vector<uint32_t> out(buf->data_as<uint32_t>(), buf->data_as<uint32_t>() + buf->size() / sizeof(uint32_t));
    log_i << "";
    system("pause");
    return 0;
}
