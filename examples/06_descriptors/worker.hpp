#pragma once

#include <chrono>
#include <thread>
#include <atomic>

class worker
{
public:
	using duration = std::chrono::duration<double>;
	using time_point = std::chrono::time_point<std::chrono::steady_clock, duration>;

	template<typename Fun>
	worker(Fun&& fun)
		: _stopped(false), _worker_thread([&] {
		_iteration_begin = std::chrono::steady_clock::now();
		while (true)
		{
			const auto delta = std::chrono::steady_clock::now() - _iteration_begin;
			_iteration_begin = std::chrono::steady_clock::now();
			if (_stopped || !fun(*this, delta))
				break;
		}
	})
	{}
	~worker()
	{
		_stopped = true;
		_worker_thread.join();
	}

	const std::atomic_bool& has_stopped() const noexcept { return _stopped; }
	void                    trigger_stop() noexcept { _stopped = true; }
	bool value_after(bool value, duration iteration_time) const noexcept
	{
		std::this_thread::sleep_for(iteration_time - (std::chrono::steady_clock::now() - _iteration_begin));
		return value;
	}

private:
	std::atomic_bool _stopped;
	time_point _iteration_begin;
	std::thread      _worker_thread;
};