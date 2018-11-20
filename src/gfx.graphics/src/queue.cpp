#include "queue.hpp"
#include "commands.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "sync.hpp"

namespace gfx {
inline namespace v1 {
void queue::submit(cref_array_view<commands> cmds, cref_array_view<semaphore> wait_for, cref_array_view<semaphore> signal,
                   opt_ref<fence const> f) const
{
    std::vector<vk::CommandBuffer> submits(cmds.size());
    for (size_t i = 0; i < submits.size(); ++i) submits[i] = cmds.data()[i].get().cmd();
    std::vector<vk::Semaphore> waits(wait_for.size());
    for (size_t i = 0; i < waits.size(); ++i) waits[i] = wait_for.data()[i].get().get_semaphore();
    std::vector<vk::Semaphore> sigs(signal.size());
    for (size_t i = 0; i < sigs.size(); ++i) sigs[i] = signal.data()[i].get().get_semaphore();

    vk::SubmitInfo submit;
    submit.commandBufferCount   = u32(submits.size());
    submit.pCommandBuffers      = submits.data();
    submit.waitSemaphoreCount   = u32(waits.size());
    submit.pWaitSemaphores      = waits.data();
    vk::PipelineStageFlags flag = vk::PipelineStageFlagBits::eAllCommands;
    submit.pWaitDstStageMask    = &flag;
    submit.signalSemaphoreCount = u32(sigs.size());
    submit.pSignalSemaphores    = sigs.data();
    _queue.submit(submit, f ? f->get().get_fence() : nullptr);
}

std::optional<present_error> queue::present(vk::ArrayProxy<std::pair<u32, std::reference_wrapper<swapchain const> const> const> swapchains,
                                            cref_array_view<semaphore> wait_for) const
{
    std::vector<u32>              images(swapchains.size());
    std::vector<vk::SwapchainKHR> swaps(swapchains.size());
    for (size_t i = 0; i < swapchains.size(); ++i)
    {
        images[i] = swapchains.data()[i].first;
        swaps[i]  = swapchains.data()[i].second.get().get_swapchain();
    }
    std::vector<vk::Semaphore> semaphores(wait_for.size());
    for (size_t i = 0; i < swapchains.size(); ++i) semaphores[i] = wait_for.data()[i].get().get_semaphore();

    vk::PresentInfoKHR const pr(u32(semaphores.size()), semaphores.data(), u32(swaps.size()), swaps.data(), images.data(), nullptr);
    vk::Result const         r = _queue.presentKHR(&pr);
    return r == vk::Result::eSuccess ? std::nullopt : std::optional<present_error>{present_error(u32(r))};
}

vk::Queue const& queue::get_queue() const noexcept
{
    return _queue;
}
void queue::wait() const noexcept
{
    _queue.waitIdle();
}
}    // namespace v1
}    // namespace gfx