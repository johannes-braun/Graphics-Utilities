#include "worker.hpp"

namespace gfx {
inline namespace v1 {
worker::~worker()
{
    stop_and_wait();
}

void worker::stop_and_wait()
{
    trigger_stop();
    if (_worker_thread.joinable())
        _worker_thread.join();
}

const std::atomic_bool& worker::finished_execution() const noexcept {
    return _finished_execution;
}
}    // namespace v1
}    // namespace gfx