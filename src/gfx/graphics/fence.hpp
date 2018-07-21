#pragma once

#include "implementation.hpp"
#include <any>
#include <chrono>

namespace gfx {
inline namespace v1 {
namespace detail {
class commands;
class fence_implementation
{
public:
	friend class commands;
	static std::unique_ptr<fence_implementation> make();

    virtual ~fence_implementation() = default;
    virtual void     wait(std::chrono::nanoseconds timeout)         = 0;
    virtual std::any api_handle()   = 0;
};
}    // namespace detail

class fence : public impl::implements<detail::fence_implementation>
{
public:
	void wait(std::chrono::nanoseconds timeout = std::chrono::nanoseconds(std::numeric_limits<long long>::max())) const { implementation()->wait(timeout); }
	std::any api_handle() const { return implementation()->api_handle(); }
};
}    // namespace v1
}    // namespace gfx
