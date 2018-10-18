#pragma once

#include "types.hpp"
#include <memory>
#include <unordered_set>
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "device.hpp"

namespace gfx {
inline namespace v1 {
template<typename T>
class mapped : public gsl::span<T>
{
public:
    using element_type           = T;
    using value_type             = std::remove_cv_t<T>;
    using index_type             = std::ptrdiff_t;
    using pointer                = element_type*;
    using reference              = element_type&;
    using iterator               = gsl::details::span_iterator<span<T, gsl::dynamic_extent>, false>;
    using const_iterator         = gsl::details::span_iterator<span<T, gsl::dynamic_extent>, true>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type              = index_type;
    using difference_type        = ptrdiff_t;

	mapped(device& d);

    template<typename = std::enable_if_t<std::is_default_constructible_v<T>>>
	mapped(device& d, size_type size);
	mapped(device& d, size_type size, T&& value);
	mapped(device& d, std::initializer_list<T> ilist);

	mapped(const mapped& other);
	mapped(mapped&& other);
	mapped& operator=(const mapped& other);
	mapped& operator=(mapped&& other);
	~mapped();

	void reserve(size_type capacity);
	void resize(size_type size, T&& value);
	void resize(size_type size, const T& value);

	void push_back(T&& value);
	void push_back(const T& value);
	void pop_back();

    template<typename... Args>
	T& emplace_back(Args&&... args);

    template<typename = std::enable_if_t<std::is_default_constructible_v<T>>>
	void resize(size_type size);

	value_type&       front();
	value_type&       back();
	const value_type& front() const;
	const value_type& back() const;
	size_type         capacity() const;
	void              shrink_to_fit();
	void              clear();

	iterator insert(const_iterator at, const T& value);
	iterator insert(const_iterator at, T&& value);
    template<class InputIt>
	iterator insert(const_iterator at, InputIt begin, InputIt end);
	iterator insert(const_iterator at, std::initializer_list<T> ilist);
	iterator erase(const_iterator at);
	iterator erase(const_iterator begin, const_iterator end);

	const vk::Buffer& get_buffer() const;

private:
	void allocate(size_type capacity, bool force = false);
	void init_range(iterator begin, iterator end, T&& value);
	void reset_storage(value_type* storage, size_type size);

    std::vector<u32> _families;
    vk::Buffer       _buffer     = nullptr;
    VmaAllocation    _allocation = nullptr;
    size_type        _capacity   = 0;
    device*          _device     = nullptr;
};

template<typename T>
class buffer
{
public:
	using element_type = T;
	using value_type = std::remove_cv_t<T>;
	using index_type = std::ptrdiff_t;
	using pointer = element_type * ;
	using reference = element_type & ;
	using size_type = index_type;
	using difference_type = ptrdiff_t;

	buffer(device& d) : _device(&d)
	{
		std::unordered_set<u32> families;
		families.emplace(_device->compute_family());
		families.emplace(_device->graphics_family());
		families.emplace(_device->transfer_family());
		_families = std::vector<u32>(families.begin(), families.end());
	}
	buffer(device& d, const mapped<T>& source)
		: buffer(d)
	{
		commands transfer_cmd = d.allocate_transfer_command();
		transfer_cmd.cmd().begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		allocate(source.size(), false, transfer_cmd.cmd());
		_size = source.size();
		transfer_cmd.cmd().copyBuffer(source.get_buffer(), _buffer, vk::BufferCopy(0, 0, _size));
		transfer_cmd.cmd().end();
		d.transfer_queue().submit({ transfer_cmd }, {}, {});
		d.transfer_queue().wait();
	}
	buffer(device& d, const mapped<T>& source, commands& transfer_cmd)
		: buffer(d)
	{
		allocate(source.size(), false, transfer_cmd.cmd());
		_size = source.size();
		transfer_cmd.cmd().copyBuffer(source.get_buffer(), _buffer, vk::BufferCopy(0, 0, _size));
	}
	buffer(device& d, const vk::ArrayProxy<const T>& proxy)
		: buffer(d)
	{
		const auto bytes = proxy.size() * sizeof(T);
		commands transfer_cmd = d.allocate_transfer_command();
		transfer_cmd.cmd().begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		allocate(proxy.size(), false, transfer_cmd.cmd());
		_size = proxy.size();
		if (bytes <= 65536)
		{
			transfer_cmd.cmd().updateBuffer(_buffer, 0, bytes, std::data(proxy));
		}
		else
		{
			mapped<T> stage(d);
			stage.insert(stage.end(), proxy.begin(), proxy.end());
			transfer_cmd.cmd().copyBuffer(stage.get_buffer(), _buffer, vk::BufferCopy(0, 0, _size));
		}
		transfer_cmd.cmd().end();
		d.transfer_queue().submit({ transfer_cmd }, {}, {});
		d.transfer_queue().wait();
	}
	buffer(device& d, const vk::ArrayProxy<const T>& proxy, commands& transfer_cmd)
		: buffer(d)
	{
		const auto bytes = proxy.size() * sizeof(T);
		allocate(proxy.size(), false, transfer_cmd.cmd());
		_size = proxy.size();
		if (bytes <= 65536)
		{
			transfer_cmd.cmd().updateBuffer(_buffer, 0, bytes, std::data(proxy));
		}
		else
		{
			mapped<T> stage(d);
			stage.insert(stage.end(), proxy.begin(), proxy.end());
			transfer_cmd.cmd().copyBuffer(stage.get_buffer(), _buffer, vk::BufferCopy(0, 0, _size));
		}
	}
	buffer(device& d, const std::initializer_list<T>& source)
		: buffer(d)
	{
		const auto bytes = std::size(source) * sizeof(T);
		commands transfer_cmd = d.allocate_transfer_command();
		transfer_cmd.cmd().begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		allocate(source.size(), false, transfer_cmd.cmd());
		_size = source.size();
		if (bytes <= 65536)
		{
			transfer_cmd.cmd().updateBuffer(_buffer, 0, bytes, std::data(source));
		}
		else
		{
			mapped<T> stage(d, source);
			transfer_cmd.cmd().copyBuffer(stage.get_buffer(), _buffer, vk::BufferCopy(0, 0, _size));
		}
		transfer_cmd.cmd().end();
		d.transfer_queue().submit(transfer_cmd, {}, {});
		d.transfer_queue().wait();
	}
	buffer(device& d, const std::initializer_list<T>& source, commands& transfer_cmd)
		: buffer(d)
	{
		const auto bytes = std::size(source) * sizeof(T);
		allocate(source.size(), false, transfer_cmd.cmd());
		_size = source.size();
		if (bytes <= 65536)
		{
			transfer_cmd.cmd().updateBuffer(_buffer, 0, bytes, std::data(source));
		}
		else
		{
			mapped<T> stage(d, source);
			transfer_cmd.cmd().copyBuffer(stage.get_buffer(), _buffer, vk::BufferCopy(0, 0, _size));
		}
	}
	~buffer() {
		if(_buffer)
			vmaDestroyBuffer(_device->alloc(), _buffer, _allocation);
	}

	buffer(const buffer& other)
		: buffer(*other._device, other.map())
	{

	}
	buffer(buffer&& other)
	{
		_buffer = std::move(other._buffer);
		_allocation = std::move(other._allocation);
		_capacity = other._capacity;
		_size = other._size;

		other._buffer = nullptr;
		other._allocation = nullptr;
		other._capacity = 0;
		other._size = 0;
	}
	buffer& operator=(const buffer& other)
	{
		_device = other._device;
		auto source = other.map();
		commands transfer_cmd = _device->allocate_transfer_command();
		transfer_cmd.cmd().begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		allocate(source.size(), false, transfer_cmd.cmd());
		_size = source.size();
		transfer_cmd.cmd().copyBuffer(source.get_buffer(), _buffer, vk::BufferCopy(0, 0, _size));
		transfer_cmd.cmd().end();
		_device->transfer_queue().submit({ transfer_cmd }, {}, {});
		_device->transfer_queue().wait();
		return *this;
	}
	buffer& operator=(buffer&& other)
	{
		_buffer = std::move(other._buffer);
		_allocation = std::move(other._allocation);
		_capacity = other._capacity;
		_size = other._size;

		other._buffer = nullptr;
		other._allocation = nullptr;
		other._capacity = 0;
		other._size = 0;
		return *this;
	}
	mapped<T> map() const {
		mapped<T> result(*_device, _size);
		commands transfer_cmd = _device->allocate_transfer_command();
		transfer_cmd.cmd().begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		transfer_cmd.cmd().copyBuffer(_buffer, result.get_buffer(), vk::BufferCopy(0, 0, _size));
		transfer_cmd.cmd().end();
		_device->transfer_queue().submit({ transfer_cmd }, {}, {});
		_device->transfer_queue().wait();
		return result;
	}
	size_type size() const { return _size; }
	size_type capacity() const { return _capacity; }
	const vk::Buffer& get_buffer() const { return _buffer; }

private:
	void allocate(size_type capacity, bool force, vk::CommandBuffer copy_cmd)
	{
		if (force || _capacity < capacity)
		{
			_capacity = capacity;
			VmaAllocationInfo       ai{};
			VmaAllocationCreateInfo aci{};
			aci.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			using bu = vk::BufferUsageFlagBits;
			vk::BufferCreateInfo bci({}, sizeof(value_type) * capacity,
				bu::eIndexBuffer | bu::eIndirectBuffer | bu::eStorageBuffer
				| bu::eStorageTexelBuffer
				/*|  bu::eConditionalRenderingEXT | bu::eRaytracingNVX | bu::eRaytracingNVX*/
				| bu::eTransferDst | bu::eTransferSrc | bu::eUniformBuffer | bu::eUniformTexelBuffer
				| bu::eVertexBuffer,
				_families.size() == 1 ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,
				u32(_families.size()), _families.data());
			VkBuffer             nb;
			VmaAllocation        na;
			vmaCreateBuffer(_device->alloc(), reinterpret_cast<VkBufferCreateInfo*>(&bci), &aci, &nb, &na, &ai);

			if(_buffer)
				copy_cmd.copyBuffer(_buffer, nb, vk::BufferCopy(0, 0, _size));

			vmaDestroyBuffer(_device->alloc(), _buffer, _allocation);
			_buffer = nb;
			_allocation = na;
		}
	}

	std::vector<u32> _families;
	vk::Buffer       _buffer = nullptr;
	VmaAllocation    _allocation = nullptr;
	size_type        _capacity = 0;
	size_type		 _size = 0;
	device*          _device = nullptr;
};

template<typename Buf> constexpr bool is_buffer_v = false;
template<typename T> constexpr bool is_buffer_v<mapped<T>> = true;
template<typename T> constexpr bool is_buffer_v<buffer<T>> = true;
}    // namespace v1
}    // namespace gfx

#include "buffer.inl"