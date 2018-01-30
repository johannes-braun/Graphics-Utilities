#pragma once

#include <chrono>

namespace jpu
{
    template<typename Ratio, typename Base = double, typename Clock = std::chrono::steady_clock>
    class timer
    {
    public:
        using base_type = Base;
        using clock_type = Clock;
        using ratio_type = Ratio;
        using duration_type = std::chrono::duration<Base, Ratio>;
        using time_point_type = std::chrono::time_point<Clock, duration_type>;

        timer() : _first_time_point(Clock::now()), _last_time_point(_first_time_point) {}

        duration_type restart()
        {
            const auto result = Clock::now() - _last_time_point;
            _last_time_point = Clock::now();
            return result;
        }

        duration_type time_since_restart() const
        {
            return Clock::now() - _last_time_point;
        }

        duration_type time_since_begin() const
        {
            return Clock::now() - _first_time_point;
        }

    private:
        time_point_type _first_time_point;
        time_point_type _last_time_point;
    };
    
    using timer_ns = timer<std::nano>;
    using timer_us = timer<std::micro>;
    using timer_ms = timer<std::milli>;
    using timer_s = timer<std::ratio<1>>;
}
