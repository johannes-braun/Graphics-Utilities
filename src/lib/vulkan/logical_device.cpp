#include "logical_device.hpp"
#include "vulkan/queue_filter.hpp"
#include "vulkan/memory.hpp"
#include "vulkan/detail.hpp"

namespace vkn
{
    LogicalDevice::LogicalDevice(LogicalDeviceCreateInfo info)
        : ClassInfo(info)
    {
        std::vector<QueueFilter> filters;
        if ((m_info.enabled_queues & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics)
            filters.push_back(QueueFilter(1.f, [&](int32_t fam, const auto& props) -> bool {
            return m_info.gpu.getSurfaceSupportKHR(fam, m_info.surface) && (props.queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics;
        }));
        if ((m_info.enabled_queues & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute)
            filters.push_back(QueueFilter(1.f, vk::QueueFlagBits::eCompute));
        if ((m_info.enabled_queues & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer)
            filters.push_back(QueueFilter(0.8f, vk::QueueFlagBits::eTransfer));
        QueueCreator queue_creator(m_info.gpu, filters);

        vk::DeviceCreateInfo device_info;
        device_info.setEnabledExtensionCount(static_cast<uint32_t>(detail::device_extensions.size()))
            .setPpEnabledExtensionNames(detail::device_extensions.data());

#if defined(NDEBUG)
#else
        device_info.setEnabledLayerCount(static_cast<uint32_t>(detail::layers.size()))
            .setPpEnabledLayerNames(detail::layers.data());
#endif

        device_info.setQueueCreateInfoCount(static_cast<uint32_t>(queue_creator.createInfos().size()))
            .setPQueueCreateInfos(queue_creator.createInfos().data());
        const auto features = m_info.gpu.getFeatures();
        device_info.setPEnabledFeatures(&features);

        Device::operator=(m_info.gpu.createDevice(device_info));

        if (!*this) throw std::runtime_error("Could not create device");

        //Query Queues and also create command buffers
        const auto make_queue = [this](uint32_t family) -> Queue {
            return { getQueue(family, 0), family };
        };

        if ((m_info.enabled_queues & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics)
        {
            m_command_pools[vk::QueueFlagBits::eGraphics] = createCommandPool({ vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                (m_queues[vk::QueueFlagBits::eGraphics] = make_queue(queue_creator.families()[0])).family });
        }
        if ((m_info.enabled_queues & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute)
        {
            m_command_pools[vk::QueueFlagBits::eCompute] = createCommandPool({ vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                (m_queues[vk::QueueFlagBits::eCompute] = make_queue(queue_creator.families()[1])).family });
        }
        if ((m_info.enabled_queues & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer)
        {
            m_command_pools[vk::QueueFlagBits::eTransfer] = createCommandPool({ {},
                (m_queues[vk::QueueFlagBits::eTransfer] = make_queue(queue_creator.families()[2])).family });
        }

        m_unique_families = queue_creator.uniqueFamilies();

        m_memory_pool = jpu::make_ref<MemoryPool>(MemoryPoolCreateInfo(this, m_info.memory_block_size));
    }

    LogicalDevice::~LogicalDevice()
    {
        for (auto&& command_pool : m_command_pools)
            destroyCommandPool(command_pool.second);
        assert(m_memory_pool->references() == 1 && "Found unused Reference to the device local memory pool. This is invalid usage.");
        m_memory_pool.reset();
        destroy();
    }

    const Queue& LogicalDevice::queue(const vk::QueueFlagBits type) const
    {
        return m_queues.at(type);
    }

    const vk::CommandPool& LogicalDevice::commandPool(const vk::QueueFlagBits type) const
    {
        return m_command_pools.at(type);
    }

    const std::vector<uint32_t>& LogicalDevice::families() const
    {
        return m_unique_families;
    }

    void LogicalDevice::oneTimeCommand(const vk::QueueFlagBits queue_type,
        const std::function<void(vk::CommandBuffer)> runnable, const vk::Fence fence) const
    {
        auto cmdbuf = allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool(queue_type), vk::CommandBufferLevel::ePrimary, 1));
        cmdbuf[0].begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        {
            runnable(cmdbuf[0]);
        }
        cmdbuf[0].end();

        queue(queue_type).queue.submit({ vk::SubmitInfo(0, nullptr, nullptr, 1, cmdbuf.data()) }, fence);
    }

    vk::PhysicalDevice LogicalDevice::gpu() const
    {
        return m_info.gpu;
    }

    MemoryPool* LogicalDevice::memory() const
    {
        return m_memory_pool.get();
    }

    LogicalDevice::operator Device() const
    {
        return static_cast<vk::Device>(*this);
    }
}
