#pragma once

#include <atomic>
#include <chrono>
#include <thread>

namespace gfx {
inline namespace v1 {
class worker
{
public:
    using duration   = std::chrono::duration<double>;
    using time_point = std::chrono::time_point<std::chrono::steady_clock, duration>;

    template<typename Fun>
    worker(Fun&& fun)
          : _stopped(false), _worker_thread([&] {
              _iteration_begin = std::chrono::steady_clock::now();
              _finished_execution = false;
              while (true)
              {
                  const auto delta = std::chrono::steady_clock::now() - _iteration_begin;
                  _iteration_begin = std::chrono::steady_clock::now();
                  if (_stopped || !fun(*this, delta)) break;
              }
              _finished_execution = true;
          })
    {}
    ~worker();

    const std::atomic_bool& has_stopped() const noexcept;
    void                    trigger_stop() noexcept;
    const std::atomic_bool& finished_execution() const noexcept;
    bool                    value_after(bool value, duration iteration_time) const noexcept;

private:
    std::atomic_bool _stopped;
    std::atomic_bool _finished_execution;
    time_point       _iteration_begin;
    std::thread      _worker_thread;
};
}    // namespace v1
}    // namespace gfx