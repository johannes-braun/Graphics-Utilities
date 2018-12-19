#pragma once
#include <atomic>
#include <functional>
#include <thread>
#include <gfx.ecs/ecs.hpp>

namespace gfx {
inline namespace v1 {
struct basic_app
{
    basic_app()                       = default;
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
        _threads[id] = std::thread {[this, fun] { (static_cast<Self*>(this)->*fun)(); }};
    }
    template<typename Fun>
    void add_thread(int id, Fun&& fun)
    {
        _threads[id] = std::thread {std::forward<Fun&&>(fun)};
    }
    void wait_until_closed()
    {
        for (auto& p : _threads)
            if (p.second.joinable()) p.second.join();
    }
    void hint_close(bool should_close) noexcept { _should_close = should_close; }
    bool should_close() const noexcept { return _should_close; }

private:
    std::atomic_bool                     _should_close = false;
    std::unordered_map<int, std::thread> _threads;
};
}    // namespace v1
}    // namespace gfx