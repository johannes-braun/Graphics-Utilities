#pragma once
#include <QTimer>
#include <QWidget>
#include <atomic>
#include <functional>
#include <gfx.ecs/ecs.hpp>
#include <shared_mutex>
#include <thread>
#include "input_qt.hpp"

namespace gfx {
inline namespace v1 {
struct basic_app
{
    enum stamp_id
    {
        stamp_id_begin = 0,
        stamp_id_shadowmap,
        stamp_id_render,
        _stamp_id_count
    };

    basic_app() : _stamp_times(_stamp_id_count)
    {
        panel.setBaseSize({800, 800});
        panel.setWindowTitle("Vulkan");
        panel.show();

        _input = std::make_unique<gfx::qt_input_system>(&panel);

        QTimer* t = new QTimer(&panel);
        QObject::connect(t, &QTimer::timeout, [=] {
            std::unique_lock l(_gui_mtx);
            for (auto& fun : _run_in_gui) fun();
            _run_in_gui.clear();
        });
        t->start(8);
    }
    basic_app(const basic_app& other) = delete;
    basic_app(basic_app&& other)      = delete;
    basic_app& operator=(const basic_app& other) = delete;
    basic_app& operator=(basic_app&& other) = delete;
    virtual ~basic_app() { wait_until_closed(); }

    void run()
    {
        add_thread(-1, [this] { on_run(); });
    }

    virtual void on_run() = 0;

    template<typename Self, typename = std::enable_if_t<std::is_convertible_v<Self*, basic_app*>>>
    void add_thread(int id, void (Self::*fun)())
    {
        if (auto it = _threads.find(id); it != _threads.end()) it->second.join();
        ++_running_threads;
        _threads[id] = std::thread{[this, fun] {
            (static_cast<Self*>(this)->*fun)();
            --_running_threads;
        }};
    }
    template<typename Fun>
    void add_thread(int id, Fun&& fun)
    {
        if (auto it = _threads.find(id); it != _threads.end()) it->second.join();
        ++_running_threads;
        _threads[id] = std::thread{[this, fun] {
            fun();
            --_running_threads;
        }};
    }
    void wait_until_closed()
    {
        for (auto& p : _threads)
            if (p.second.joinable()) p.second.join();
    }
    void hint_close(bool should_close) noexcept { _should_close = should_close; }
    bool should_close() const noexcept { return _should_close; }
    bool finished() const noexcept { return _running_threads.load() <= 0; }

    std::chrono::nanoseconds current_frametime() const noexcept { return std::chrono::nanoseconds(_frame_nanoseconds); }
    void                     update_frametime(std::chrono::nanoseconds nanos) { _frame_nanoseconds = nanos.count(); }

    const std::vector<std::uint64_t>& stamp_times() const noexcept { return _stamp_times; }
    std::shared_mutex&                stamp_time_mutex() { return _stamp_time_mutex; }

    void* user_data = nullptr;

protected:
    std::shared_mutex          _stamp_time_mutex;
    std::vector<std::uint64_t> _stamp_times;
    QWidget                              panel;
    std::unique_ptr<gfx::qt_input_system> _input;

    void run_in_gui(std::function<void()> fun)
    {
        std::unique_lock l(_gui_mtx);
        _run_in_gui.emplace_back(std::move(fun));
    }

private:
    std::mutex                         _gui_mtx;
    std::vector<std::function<void()>> _run_in_gui;

    std::atomic_bool                     _should_close    = false;
    std::atomic_int                      _running_threads = 0;
    std::unordered_map<int, std::thread> _threads;
    std::atomic_size_t                   _frame_nanoseconds;
};
}    // namespace v1
}    // namespace gfx