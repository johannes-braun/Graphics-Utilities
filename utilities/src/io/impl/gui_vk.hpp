#pragma once
#include <imgui/imgui.h>
#include <vulkan/vulkan.hpp>
#include <cinttypes>
//#include <vulkan/memory.hpp>

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
        gui_vk() : _init(false) {};
        gui_vk(vkn::device* device, vkn::swapchain* swapchain);
        ~gui_vk();
        ImTextureID build_font_atlas();
        void pre_render(ImDrawData* draw_data);
        void render(const ImDrawCmd& cmd, int index_offset, int vertex_offset);
        void post_render() const;

    private:
        bool _init{ true };

        //void update_swapchain(vkn::swapchain* swapchain);
        //void set_next_command_buffer(vk::CommandBuffer buffer);
        //void create_device_objects();

        //vkn::device* _device{ nullptr };
        //vkn::swapchain* _swapchain{ nullptr };

        //// Device Objects
        //vk::Sampler _font_sampler;
        //vk::DescriptorSetLayout _descriptor_set_layout;
        //vk::PipelineLayout _pipeline_layout;
        //vk::Pipeline _pipeline;
        //vk::DescriptorPool _descriptor_pool;

        //// Font Atlas
        //vk::Image _font_image;
        //const vkn::memory_block* _font_image_memory{ nullptr };
        //vk::ImageView _font_image_view;

        //vk::DescriptorImageInfo _font_descriptor_image;
        //vk::WriteDescriptorSet _font_write_descriptor;

        //struct FrameInfo
        //{
        //    const vkn::memory_block* buffer_memory{ nullptr };
        //    vk::DeviceSize last_size = 0;
        //    vk::DeviceSize idx_offset = 0;
        //    vk::Buffer vertex_buffer{ nullptr };
        //    vk::Buffer index_buffer{ nullptr };
        //};
        //
        //vk::CommandBuffer _command_buffer;
        //uint32_t _current_frame = 0;
        //std::vector<FrameInfo> _frame_infos = std::vector<FrameInfo>(queued_frames);

        //vk::RenderPass _renderpass;
        //std::vector<vk::Framebuffer> _framebuffers;
    };
}
