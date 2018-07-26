#pragma once

#include "../general/handle.hpp"
#include "../sampler.hpp"
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {
class sampler_implementation : public detail::sampler_implementation
{
public:
    sampler_implementation();
    ~sampler_implementation();
    void set_filter(filter_mode mode, filter filter) override;
    void set_wrap(wrap w, wrap_mode mode) override;
    void set_border(border_color color) override;
    void set_lod(lod mode, float value) override;
    void set_anisotropy(bool enable, float value) override;
    void set_compare(bool enable, compare_op op) override;

    std::any api_handle() override;

private:
    movable_handle<mygl::sampler> _handle        = mygl::sampler::zero;
    filter                        _min_filter    = filter::nearest;
    filter                        _mipmap_filter = filter::nearest;
};
}    // namespace opengl
}    // namespace v1
}    // namespace gfx