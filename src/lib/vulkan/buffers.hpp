#pragma once
#include "vulkan/vulkan.hpp"
#include "memory.hpp"
#include "logical_device.hpp"
#include <jpu/memory>

namespace vkn
{
    struct UniformBufferCreateInfo
    {
        UniformBufferCreateInfo(LogicalDevice* device = nullptr)
            : device(device){}

        UniformBufferCreateInfo& setDevice(LogicalDevice* value) { device = value; return *this; }

        LogicalDevice* device;    
    };

	template<typename T>
	class UniformBuffer : ClassInfo<UniformBufferCreateInfo, UniformBuffer<T>>, public jpu::ref_count
	{
	public:
		UniformBuffer(const UniformBufferCreateInfo& info);
		~UniformBuffer();

        T& at(size_t idx);
        const T& at(size_t idx) const;

		void flush(const vk::DeviceSize index = 0, const vk::DeviceSize count = 1) const;

		vk::Buffer buffer() const;

	private:
		vk::Buffer m_buffer;
		MemoryBlock m_buffer_memory;
	};

	template <typename T>
	UniformBuffer<T>::UniformBuffer(const UniformBufferCreateInfo& info)
		: ClassInfo(info)
	{
        m_info.device->incRef();
		m_buffer = m_info.device->createBuffer(vk::BufferCreateInfo({}, sizeof(T), vk::BufferUsageFlagBits::eUniformBuffer));
		const auto mvp_requirements = m_info.device->getBufferMemoryRequirements(m_buffer);
		m_buffer_memory = m_info.device->memory()->allocate(mvp_requirements.size, mvp_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible);
		m_info.device->bindBufferMemory(m_buffer, m_buffer_memory->memory, m_buffer_memory->offset);
	}

	template <typename T>
	UniformBuffer<T>::~UniformBuffer()
	{
		m_info.device->memory()->free(m_buffer_memory);
		m_info.device->destroyBuffer(m_buffer);
        m_info.device->decRef();
	}

    template <typename T>
    T& UniformBuffer<T>::at(const size_t idx)
    {
        return *(static_cast<T*>(m_buffer_memory->data) + idx);
    }

    template <typename T>
    const T& UniformBuffer<T>::at(const size_t idx) const
    {
        return *(static_cast<T*>(m_buffer_memory->data) + idx);
    }

 /*   template <typename T>
	T& UniformBuffer<T>::mapSingle(const vk::DeviceSize index)
	{
		return *reinterpret_cast<T*>(m_info.device->mapMemory(m_buffer_memory->memory, m_buffer_memory->offset + index, sizeof(T), {}));
	}

	template <typename T>
	T* UniformBuffer<T>::mapMultiple(const vk::DeviceSize index, const vk::DeviceSize count)
	{
		return reinterpret_cast<T*>(m_info.device->mapMemory(m_buffer_memory->memory, m_buffer_memory->offset + index, count * sizeof(T), {}));
	}*/

	template <typename T>
	void UniformBuffer<T>::flush(const vk::DeviceSize index, const vk::DeviceSize count) const
	{
		m_info.device->flushMappedMemoryRanges({ vk::MappedMemoryRange(m_buffer_memory->memory, m_buffer_memory->offset + index, count * sizeof T) });
	}

  /*  template <typename T>
    void UniformBuffer<T>::unmap() const
    {
		m_info.device->unmapMemory(m_buffer_memory->memory);
    }*/

    template <typename T>
	vk::Buffer UniformBuffer<T>::buffer() const
	{
		return m_buffer;
	}
}
