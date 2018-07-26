/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKDeferredPipelines.cpp
 *   DATE: Wed Jul 18 21:34:29 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/RenderControl/Pipelines/VKCompositionPipeline.h"
#include "Vulkan/Shapes/VKShapeFactory.h"
#include "Vulkan/Core/VulkanUtilities.h"

/***********************************************************************
 *  Method: VKCompositionPipeline::VKCompositionPipeline
 *  Params: const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int &a_subpassIndex, const glm::vec2& a_res, const glm::vec4 &a_viewportSettings
 * Effects: 
 ***********************************************************************/
VKCompositionPipeline::VKCompositionPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, 
                                                           const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int &a_subpassIndex, 
                                                           const glm::vec2& a_res, const glm::vec4 &a_viewportSettings, const unsigned int& a_inSamplersCount)
 : VKPipeline(a_logicalDevice, a_renderPass, a_shaders, a_subpassIndex, a_res, a_viewportSettings), m_inSamplersCount(a_inSamplersCount)
{
}
/***********************************************************************
 *  Method: VKCompositionPipeline::Init
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKCompositionPipeline::Init()
{
  VkVertexInputBindingDescription l_bindingDesc = VKShapeFactory::GetBindingDescription();
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.vertexAttributeDescriptionCount = VKShapeFactory::GetAttributeDescriptions().size();
  vertexInputInfo.pVertexBindingDescriptions = &l_bindingDesc;
  vertexInputInfo.pVertexAttributeDescriptions = VKShapeFactory::GetAttributeDescriptions().data();
  
  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;
  
  VkViewport viewport = {};
  viewport.x = m_viewportSettings.x;
  viewport.y = m_viewportSettings.y;
  viewport.width = m_viewportSettings.z;
  viewport.height = m_viewportSettings.w;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  
  VkRect2D scissor = {};
  scissor.offset = {0, 0};
  scissor.extent = { (uint32_t)m_res.x, (uint32_t)m_res.y};
  
  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;
  
  
  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_NONE;    // VK_CULL_MODE_BACK_BIT 
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  
  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  
  
  VkPipelineDepthStencilStateCreateInfo depthStencil = {};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  
  depthStencil.stencilTestEnable = VK_FALSE;
  
  depthStencil.front = {};
  depthStencil.back = {};
  

  

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

    
  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;
  
  // create descripor set layout bindinds
  VkDescriptorSetLayoutBinding l_vertexBindingGlobal = GetUniformVertexLayoutBinding(0);
  VkDescriptorSetLayoutBinding l_vertexBindingObject = GetUniformVertexLayoutBinding(1);
  VkDescriptorSetLayoutBinding l_samplerObject = GetSamplerFragmentLayoutBinding(2);
  
  m_descLayoutBindings = {l_vertexBindingGlobal, l_vertexBindingObject, l_samplerObject};
  m_descriptorSetLayout = CreateDescriptorSetLayout( m_logicalDevice->GetDevice(), m_descLayoutBindings );

  
  
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;

  pipelineLayoutInfo.pushConstantRangeCount = 0;
  
  if (vkCreatePipelineLayout(m_logicalDevice->GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    throw std::runtime_error("VKCompositionPipeline::Init() - failed to create pipeline layout!");
  
  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = m_shaders.size();
  pipelineInfo.pStages = m_shaders.data();
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.layout = m_pipelineLayout;
  pipelineInfo.renderPass = m_renderPass;
  pipelineInfo.subpass = m_subpassIndex;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  
  if (vkCreateGraphicsPipelines(m_logicalDevice->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline )  != VK_SUCCESS)
    throw std::runtime_error("VKCompositionPipeline::Init() - failed to create graphics pipeline!");
}
