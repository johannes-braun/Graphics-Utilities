#pragma once

#include <vulkan/vulkan.h>

namespace gfx
{
    class vertex_input_state
    {
    public:
        vertex_input_state() {
            
        }

    private:

    };

    class graphics_pipeline_creator
    {
    public:
        graphics_pipeline(VkPipelineCreateFlags flag)

    private:
    };
}
//VkPipelineInputAssemblyStateCreateInfo;
//VkPipelineVertexInputStateCreateInfo;
/*
VkStructureType                                  sType;
const void*                                      pNext;
VkPipelineCreateFlags                            flags;
uint32_t                                         stageCount;
const VkPipelineShaderStageCreateInfo*           pStages;
const VkPipelineVertexInputStateCreateInfo*      pVertexInputState;
const VkPipelineInputAssemblyStateCreateInfo*    pInputAssemblyState;
const VkPipelineTessellationStateCreateInfo*     pTessellationState;
const VkPipelineViewportStateCreateInfo*         pViewportState;
const VkPipelineRasterizationStateCreateInfo*    pRasterizationState;
const VkPipelineMultisampleStateCreateInfo*      pMultisampleState;
const VkPipelineDepthStencilStateCreateInfo*     pDepthStencilState;
const VkPipelineColorBlendStateCreateInfo*       pColorBlendState;
const VkPipelineDynamicStateCreateInfo*          pDynamicState;
VkPipelineLayout                                 layout;
VkRenderPass                                     renderPass;
uint32_t                                         subpass;
VkPipeline                                       basePipelineHandle;
int32_t                                          basePipelineIndex;
*/