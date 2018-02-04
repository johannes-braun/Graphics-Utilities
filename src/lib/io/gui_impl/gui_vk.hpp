#pragma once
#include "imgui/imgui.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/memory.hpp"

namespace vkn {
    class device;
    class swapchain;
}

namespace io
{
    class window;
}

namespace io::impl
{
    class gui_vk
    {
    public:
        friend window;
        constexpr static uint32_t queued_frames = 2;

        gui_vk(vkn::device* device, vkn::swapchain* swapchain);
        ~gui_vk();
        ImTextureID build_font_atlas();
        void pre_render(ImDrawData* draw_data);
        void render(const ImDrawCmd& cmd, int index_offset, int vertex_offset);
        void post_render();

    private:
        void update_swapchain(vkn::swapchain* swapchain);
        void set_next_command_buffer(vk::CommandBuffer buffer);
        void create_device_objects();

        vkn::device* _device;
        vkn::swapchain* _swapchain;

        // Device Objects
        vk::Sampler m_font_sampler;
        vk::DescriptorSetLayout m_descriptor_set_layout;
        vk::PipelineLayout m_pipeline_layout;
        vk::Pipeline m_pipeline;
        vk::DescriptorPool m_descriptor_pool;

        // Font Atlas
        vk::Image m_font_image;
        const vkn::memory_block* m_font_image_memory;
        vk::ImageView m_font_image_view;

        vk::DescriptorImageInfo m_font_descriptor_image;
        vk::WriteDescriptorSet m_font_write_descriptor;

        struct FrameInfo
        {
            const vkn::memory_block* buffer_memory{ nullptr };
            vk::DeviceSize last_size = 0;
            vk::DeviceSize idx_offset = 0;
            vk::Buffer vertex_buffer{ nullptr };
            vk::Buffer index_buffer{ nullptr };
        };
        
        vk::CommandBuffer _command_buffer;
        uint32_t _current_frame = 0;
        std::vector<FrameInfo> m_frame_infos = std::vector<FrameInfo>(queued_frames);

        vk::RenderPass m_renderpass;
        std::vector<vk::Framebuffer> m_framebuffers;
    };
}
