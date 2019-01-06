#pragma once
#include "app.hpp"
#include <vulkan/vulkan.hpp>
#include <gfx.graphics/vulkan/image.hpp>
#include <gfx.graphics/vulkan/buffer.hpp>

struct vulkan_state_t
{
    vk::UniqueRenderPass               render_pass;
    std::vector<vk::UniqueFramebuffer> framebuffers;
    std::vector<gfx::vulkan::image>    msaa_attachments;
    std::vector<gfx::vulkan::image>    depth_attachments;
    std::vector<vk::UniqueImageView>   color_att_views;
    std::vector<vk::UniqueImageView>   msaa_att_views;
    std::vector<vk::UniqueImageView>   depth_att_views;
    vk::UniqueDescriptorSetLayout      cam_buffer_layout;
    vk::UniqueDescriptorSetLayout      model_info_layout;

    std::uint32_t                 texture_count;
    vk::UniqueSampler             default_texture_sampler;
    vk::UniqueDescriptorSetLayout all_textures_layout;

    vk::UniqueRenderPass          shadow_render_pass;
    vk::UniqueSampler             shadow_sampler;
    vk::UniqueDescriptorSetLayout shadow_set_layout;
};

class vulkan_app : public gfx::basic_app
{
public:
    void on_run() override;

private:
    struct att_id
    {
        enum
        {
            color,
            msaa,
            depth,
            _count
        };
    };

    void                     create_shadow_renderpass(gfx::vulkan::device& gpu);
    void                     create_renderpass(gfx::vulkan::device& gpu, gfx::vulkan::swapchain& swapchain);
    vk::UniquePipelineLayout create_pipeline_layout(gfx::vulkan::device& gpu);
    vk::UniquePipeline       create_pipeline(gfx::vulkan::device& gpu, vk::PipelineLayout layout);
    vk::UniquePipelineLayout create_shadow_pipeline_layout(gfx::vulkan::device& gpu);
    vk::UniquePipeline       create_shadow_pipeline(gfx::vulkan::device& gpu, vk::PipelineLayout layout);

    void create_framebuffer(gfx::vulkan::device& gpu, gfx::vulkan::swapchain& swapchain);
    void generate_mipmaps(gfx::vulkan::device& gpu, vk::Image image, std::uint32_t layers, std::uint32_t levels, vk::Extent3D extent,
                          const gfx::vulkan::mapped<std::byte>& data);
    std::unique_ptr<vulkan_state_t> vulkan_state;
};