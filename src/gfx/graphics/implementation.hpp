#pragma once
#include <any>
#include <cassert>
#include <memory>
#include <gfx/api.hpp>

namespace gfx {
inline namespace v1 {
namespace impl {


template<typename Implementation>
class implements
{
public:
    static constexpr auto creator_function = Implementation::make;
    using implementation_type              = Implementation;
    using handle_type                      = handle;

    implements() : _implementation(creator_function()) { assert(_implementation); }

	implements(implements&& i) noexcept : _implementation(std::move(i._implementation)) {  }
	implements& operator=(implements&& i) noexcept { _implementation = std::move(i._implementation); return *this; }

    handle_type api_handle() const { return _implementation->api_handle(); }

	// TODO protected:
    const std::unique_ptr<implementation_type>& implementation() const { return _implementation; }

protected:
    // TODO private:
    std::unique_ptr<implementation_type> _implementation;
};


}    // namespace impl
}    // namespace v1
}    // namespace gfx