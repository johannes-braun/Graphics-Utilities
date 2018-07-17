#pragma once
#include <memory>
#include <any>
#include <cassert>

namespace gfx::detail
{
namespace base
{
template<typename Implementation>
class implements
{
public:
    static constexpr auto creator_function = Implementation::make;
    using implementation_type              = Implementation;
    using handle_type                      = std::any;

    implements() : _implementation(creator_function()) { assert(_implementation); }

    handle_type api_handle() const { return _implementation->api_handle(); }

protected:
    const std::unique_ptr<implementation_type>& implementation() const { return _implementation; }

    // TODO private:
    std::unique_ptr<implementation_type> _implementation;
};
}    // namespace base
}
