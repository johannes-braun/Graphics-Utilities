#pragma once

#include <context.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {
class context_implementation : public detail::context_implementation
{
public:
    void initialize(GLFWwindow* window, const context_options& options) override;
    void make_current(GLFWwindow* window) override;
};
}    // namespace opengl
}    // namespace v1
}    // namespace gfx