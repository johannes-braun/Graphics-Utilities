#include "worker.hpp"

namespace gfx {
inline namespace v1 {
worker::~worker()
{
    _stopped = true;
    _worker_thread.join();
}

const std::atomic_bool& worker::has_stopped() const noexcept
{
    return _stopped;
}
void worker::trigger_stop() noexcept
{
    _stopped = true;
}
bool worker::value_after(bool value, duration iteration_time) const noexcept
{
    std::this_thread::sleep_for(iteration_time - (std::chrono::steady_clock::now() - _iteration_begin));
    return value;
}
}    // namespace v1
}    // namespace gfx