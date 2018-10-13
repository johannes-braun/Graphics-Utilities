#include "queue.hpp"
#include "device.hpp"
#include "sync.hpp"
#include "swapchain.hpp"
#include "commands.hpp"

namespace gfx
{
	inline namespace v1
	{

		void queue::submit(cref_array_view<commands> cmds, cref_array_view<semaphore> wait_for, cref_array_view<semaphore> signal,
			opt_ref<const fence> f) const
		{
			std::vector<vk::CommandBuffer> submits(cmds.size());
			for (size_t i = 0; i < submits.size(); ++i) submits[i] = cmds.data()[i].get().cmd();
			std::vector<vk::Semaphore> waits(wait_for.size());
			for (size_t i = 0; i < waits.size(); ++i) waits[i] = wait_for.data()[i].get().sem();
			std::vector<vk::Semaphore> sigs(signal.size());
			for (size_t i = 0; i < sigs.size(); ++i) sigs[i] = signal.data()[i].get().sem();

			vk::SubmitInfo submit;
			submit.commandBufferCount = u32(submits.size());
			submit.pCommandBuffers = submits.data();
			submit.waitSemaphoreCount = u32(waits.size());
			submit.pWaitSemaphores = waits.data();
			vk::PipelineStageFlags flag = vk::PipelineStageFlagBits::eAllCommands;
			submit.pWaitDstStageMask = &flag;
			submit.signalSemaphoreCount = u32(sigs.size());
			submit.pSignalSemaphores = sigs.data();
			_queue.submit(submit, f ? f->get().fen() : nullptr);
		}

		std::optional<present_error> queue::present(vk::ArrayProxy<const std::pair<u32, const std::reference_wrapper<const swapchain>>> swapchains,
			cref_array_view<semaphore> wait_for) const
		{
			std::vector<u32>              images(swapchains.size());
			std::vector<vk::SwapchainKHR> swaps(swapchains.size());
			for (size_t i = 0; i < swapchains.size(); ++i)
			{
				images[i] = swapchains.data()[i].first;
				swaps[i] = swapchains.data()[i].second.get().chain();
			}
			std::vector<vk::Semaphore> semaphores(wait_for.size());
			for (size_t i = 0; i < swapchains.size(); ++i) semaphores[i] = wait_for.data()[i].get().sem();

			const vk::PresentInfoKHR pr(u32(semaphores.size()), semaphores.data(), u32(swaps.size()), swaps.data(), images.data(), nullptr);
			const vk::Result         r = _queue.presentKHR(&pr);
			return r == vk::Result::eSuccess ? std::nullopt : std::optional<present_error>{ present_error(u32(r)) };
		}

		const vk::Queue& queue::get() const noexcept { return _queue; }
		void             queue::wait() const noexcept { _queue.waitIdle(); }
	}
}