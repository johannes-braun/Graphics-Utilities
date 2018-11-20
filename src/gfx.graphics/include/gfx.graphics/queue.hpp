#pragma once

#include "types.hpp"
#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {
class semaphore;
class fence;
class swapchain;
class commands;

enum class present_error
{
    out_of_date = -1000001004,
    suboptimal  = 1000001003
};

class queue
{
public:
    queue() = delete;

    void submit(cref_array_view<commands> cmds, cref_array_view<semaphore> wait_for, cref_array_view<semaphore> signal,
                opt_ref<fence const> f = std::nullopt) const;
    [[nodiscard]] std::optional<present_error>
        present(vk::ArrayProxy<std::pair<u32, std::reference_wrapper<swapchain const> const> const> swapchains,
                cref_array_view<semaphore>                                                          wait_for) const;

    [[nodiscard]] auto get_queue() const noexcept -> vk::Queue const&;
    void               wait() const noexcept;

private:
    vk::Queue _queue;
};
}    // namespace v1
}    // namespace gfx