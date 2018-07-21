#pragma once
#include <any>
#include <cassert>
#include <memory>

namespace gfx {
inline namespace v1 {
namespace impl {


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


}    // namespace impl
}    // namespace v1
}    // namespace gfx