#pragma once

#include <atomic>
#include <chrono>
#include <thread>

namespace gfx {
inline namespace v1 {
class timed_while
{
public:
    using duration   = std::chrono::duration<double>;
    using time_point = std::chrono::time_point<std::chrono::steady_clock, duration>;

    template<typename Fun>
    static void run(Fun&& fun)
    {
        timed_while{}.run_impl(std::forward<Fun&&>(fun));
    }

    bool value_after(bool value, duration iteration_time) const noexcept
    {
        std::this_thread::sleep_for(iteration_time - (std::chrono::steady_clock::now() - _iteration_begin));
        return value;
    }
    bool has_stopped() const noexcept { return _stopped; }
    void trigger_stop() noexcept { _stopped = true; }

protected:
    timed_while() = default;
    template<typename Fun>
    void run_impl(Fun&& fun)
    {
        _iteration_begin = std::chrono::steady_clock::now();
        while (true)
        {
            const auto delta = std::chrono::steady_clock::now() - _iteration_begin;
            _iteration_begin = std::chrono::steady_clock::now();
            if (_stopped || !fun(*this, delta)) break;
        }
    }

private:
    std::atomic_bool _stopped = false;
    time_point _iteration_begin;
};

class worker final : public timed_while
{
public:
    template<typename Fun>
    explicit worker(Fun&& fun)
          : _worker_thread([&, this] {
              _finished_execution = false;
              timed_while::run_impl(fun);
              _finished_execution = true;
          })
    {}
    ~worker();
    void                    stop_and_wait();
    const std::atomic_bool& finished_execution() const noexcept;

private:
    std::atomic_bool _finished_execution = false;
    std::thread      _worker_thread;
};
}    // namespace v1
}    // namespace gfx