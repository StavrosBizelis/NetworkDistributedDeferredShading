/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKDeferredPipelines.cpp
 *   DATE: Wed Jul 18 21:34:29 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/RenderControl/Pipelines/VKDeferredPipelines.h"
#include "Vulkan/Shapes/VKShapeFactory.h"
#include "Vulkan/Core/VulkanUtilities.h"

/***********************************************************************
 *  Method: VKGeometryPassPipeline::VKGeometryPassPipeline
 *  Params: const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int &a_subpassIndex, const glm::vec2& a_res, const glm::vec4 &a_viewportSettings
 * Effects: 
 ***********************************************************************/
VKGeometryPassPipeline::VKGeometryPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, 
                                                           const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int &a_subpassIndex, 
                                                           const glm::vec2& a_res, const glm::vec4 &a_viewportSettings, const unsigned int& a_inSamplersCount)
 : VKPipeline(a_logicalDevice, a_renderPass, a_shaders, a_subpassIndex, a_res, a_viewportSettings), m_inSamplersCount(a_inSamplersCount)
{
}
/***********************************************************************
 *  Method: VKGeometryPassPipeline::Init
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKGeometryPassPipeline::Init()
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
  rasterizer.cullMode =  VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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


  std::vector< VkPipelineColorBlendAttachmentState > l_colourBlendAttachments = {colorBlendAttachment, colorBlendAttachment, colorBlendAttachment, colorBlendAttachment};
  
  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = l_colourBlendAttachments.size();
  colorBlending.pAttachments = l_colourBlendAttachments.data();
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;
  
  // create descripor set layout bindinds
  VkDescriptorSetLayoutBinding l_vertexBindingGlobal = GetUniformVertexLayoutBinding(0);
  VkDescriptorSetLayoutBinding l_vertexBindingObject = GetUniformVertexLayoutBinding(1);
  
  VkDescriptorSetLayoutBinding l_fragUniform = GetUniformFragmentLayoutBinding(2);
  m_descLayoutBindings = {l_vertexBindingGlobal, l_vertexBindingObject, l_fragUniform};
  
  for( unsigned int i = 0; i < m_inSamplersCount; ++i)
    m_descLayoutBindings.push_back( GetSamplerFragmentLayoutBinding(3+i) );
  
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
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.layout = m_pipelineLayout;
  pipelineInfo.renderPass = m_renderPass;
  pipelineInfo.subpass = m_subpassIndex;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  
  if (vkCreateGraphicsPipelines(m_logicalDevice->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline )  != VK_SUCCESS)
    throw std::runtime_error("VKCompositionPipeline::Init() - failed to create graphics pipeline!");
  
  
}








/***********************************************************************
 *  Method: VKSkyboxPassPipeline::VKSkyboxPassPipeline
 *  Params: const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int &a_subpassIndex, const glm::vec2& a_res, const glm::vec4 &a_viewportSettings
 * Effects: 
 ***********************************************************************/
VKSkyboxPassPipeline::VKSkyboxPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, 
                                                           const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int &a_subpassIndex, 
                                                           const glm::vec2& a_res, const glm::vec4 &a_viewportSettings)
 : VKPipeline(a_logicalDevice, a_renderPass, a_shaders, a_subpassIndex, a_res, a_viewportSettings)
{
}
/***********************************************************************
 *  Method: VKSkyboxPassPipeline::Init
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKSkyboxPassPipeline::Init()
{
  VkVertexInputBindingDescription l_bindingDesc = VKShapeFactory::GetBindingDescription();
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(VKShapeFactory::GetAttributeDescriptions().size() );
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
  rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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
  
  
  std::vector <VkPipelineColorBlendAttachmentState> l_attachmentsBlend(4);
  for( unsigned int i = 0; i < l_attachmentsBlend.size(); ++i)
  {
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    l_attachmentsBlend[i] = colorBlendAttachment;
  }
  
  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = l_attachmentsBlend.size();
  colorBlending.pAttachments = l_attachmentsBlend.data();
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;
  
  // create descripor set layout bindinds
  VkDescriptorSetLayoutBinding l_vertexBindingGlobal = GetUniformVertexLayoutBinding(0);
  VkDescriptorSetLayoutBinding l_vertexBindingObject = GetUniformVertexLayoutBinding(1);
  VkDescriptorSetLayoutBinding l_sampler1 = GetSamplerFragmentLayoutBinding(2);
  
  m_descLayoutBindings = {l_vertexBindingGlobal, l_vertexBindingObject, l_sampler1};
  // simple geometry
  m_descriptorSetLayout = CreateDescriptorSetLayout( m_logicalDevice->GetDevice(), m_descLayoutBindings );

  
  
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;

  pipelineLayoutInfo.pushConstantRangeCount = 0;
  
  if (vkCreatePipelineLayout(m_logicalDevice->GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    throw std::runtime_error("VKSkyboxPassPipeline::Init() - failed to create pipeline layout!");
  
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
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.layout = m_pipelineLayout;
  pipelineInfo.renderPass = m_renderPass;
  pipelineInfo.subpass = m_subpassIndex;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  
  if (vkCreateGraphicsPipelines(m_logicalDevice->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline )  != VK_SUCCESS)
    throw std::runtime_error("VKSkyboxPassPipeline::Init() - failed to create graphics pipeline!");
}













/***********************************************************************
 *  Method: VKLightPassPipeline::VKLightPassPipeline
 *  Params: const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int &a_subpassIndex, const glm::vec2& a_res, const glm::vec4 &a_viewportSettings
 * Effects: 
 ***********************************************************************/
VKLightPassPipeline::VKLightPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, 
                                         const unsigned int &a_subpassIndex, const glm::vec2& a_res, const glm::vec4 &a_viewportSettings, bool a_isSpot)
 : VKPipeline(a_logicalDevice, a_renderPass, a_shaders, a_subpassIndex, a_res, a_viewportSettings), m_spot(a_isSpot)
{
}


/***********************************************************************
 *  Method: VKDirLightPassPipeline::Init
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKLightPassPipeline::Init()
{
  VkVertexInputBindingDescription l_bindingDesc = VKShapeFactory::GetBindingDescription();
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(VKShapeFactory::GetAttributeDescriptions().size() );
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
  rasterizer.depthClampEnable = VK_TRUE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  
  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  
  
  VkPipelineDepthStencilStateCreateInfo depthStencil = {};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_FALSE;
  depthStencil.depthWriteEnable = VK_FALSE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  
  // normal for light is true
  // depthStencil.stencilTestEnable = VK_TRUE;
  depthStencil.stencilTestEnable = VK_FALSE;
  
  depthStencil.front = {};
  depthStencil.back = {};

  

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_TRUE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


  std::vector< VkPipelineColorBlendAttachmentState > l_colourBlendAttachments = { colorBlendAttachment };
  
  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = l_colourBlendAttachments.size();
  colorBlending.pAttachments = l_colourBlendAttachments.data();
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;
  
  
  
  
  
  // create descripor set layout bindinds
  VkDescriptorSetLayoutBinding l_vertexBindingGlobal = GetUniformVertexLayoutBinding(0);
  VkDescriptorSetLayoutBinding l_vertexBindingObject = GetUniformVertexLayoutBinding(1);
  VkDescriptorSetLayoutBinding l_fragBindingGlobal = GetUniformFragmentLayoutBinding(2);
  VkDescriptorSetLayoutBinding l_fragBindingObject = GetUniformFragmentLayoutBinding(3);
  VkDescriptorSetLayoutBinding l_sampler1 = GetInputAttachmentFragmentLayoutBinding(4);
  VkDescriptorSetLayoutBinding l_sampler2 = GetInputAttachmentFragmentLayoutBinding(5);
  VkDescriptorSetLayoutBinding l_sampler3 = GetInputAttachmentFragmentLayoutBinding(6);
  VkDescriptorSetLayoutBinding l_sampler4 = GetInputAttachmentFragmentLayoutBinding(7);
  
  m_descLayoutBindings = { l_vertexBindingGlobal, l_fragBindingGlobal, l_vertexBindingObject, l_fragBindingObject, l_sampler1, l_sampler2, l_sampler3, l_sampler4 };
  m_descriptorSetLayout = CreateDescriptorSetLayout( m_logicalDevice->GetDevice(), m_descLayoutBindings ); 


  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;

  pipelineLayoutInfo.pushConstantRangeCount = 0;
  if (vkCreatePipelineLayout(m_logicalDevice->GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    throw std::runtime_error("VKDirLightPassPipeline::Init() - failed to create pipeline layout!");
  
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
    throw std::runtime_error("VKDirLightPassPipeline::Init() - failed to create graphics pipeline!");
}








/***********************************************************************
 *  Method: VKDirLightPassPipeline::VKDirLightPassPipeline
 *  Params: const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int &a_subpassIndex, const glm::vec2& a_res, const glm::vec4 &a_viewportSettings
 * Effects: 
 ***********************************************************************/
VKDirLightPassPipeline::VKDirLightPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int &a_subpassIndex, const glm::vec2& a_res, const glm::vec4 &a_viewportSettings)
 : VKPipeline(a_logicalDevice, a_renderPass, a_shaders, a_subpassIndex, a_res, a_viewportSettings)
{
}


/***********************************************************************
 *  Method: VKDirLightPassPipeline::Init
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKDirLightPassPipeline::Init()
{
  
  VkVertexInputBindingDescription l_bindingDesc = VKShapeFactory::GetBindingDescription();
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(VKShapeFactory::GetAttributeDescriptions().size() );
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
  rasterizer.depthClampEnable = VK_TRUE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  
  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  
  
  VkPipelineDepthStencilStateCreateInfo depthStencil = {};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_FALSE;
  depthStencil.depthWriteEnable = VK_FALSE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  
  // normal for light is true
  // depthStencil.stencilTestEnable = VK_TRUE;
  depthStencil.stencilTestEnable = VK_FALSE;
  
  depthStencil.front = {};
  depthStencil.back = {};

  

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_TRUE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


  std::vector< VkPipelineColorBlendAttachmentState > l_colourBlendAttachments = { colorBlendAttachment };
  
  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = l_colourBlendAttachments.size();
  colorBlending.pAttachments = l_colourBlendAttachments.data();
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;
  
  // create descripor set layout bindinds
  VkDescriptorSetLayoutBinding l_fragBindingGlobal = GetUniformFragmentLayoutBinding(0);
  VkDescriptorSetLayoutBinding l_fragBindingObject = GetUniformFragmentLayoutBinding(1);
  VkDescriptorSetLayoutBinding l_sampler1 = GetInputAttachmentFragmentLayoutBinding(2);
  VkDescriptorSetLayoutBinding l_sampler2 = GetInputAttachmentFragmentLayoutBinding(3);
  VkDescriptorSetLayoutBinding l_sampler3 = GetInputAttachmentFragmentLayoutBinding(4);
  VkDescriptorSetLayoutBinding l_sampler4 = GetInputAttachmentFragmentLayoutBinding(5);
  
  m_descLayoutBindings = { l_fragBindingGlobal, l_fragBindingObject, l_sampler1, l_sampler2, l_sampler3, l_sampler4 };
  m_descriptorSetLayout = CreateDescriptorSetLayout( m_logicalDevice->GetDevice(), m_descLayoutBindings ); 

  
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.pNext = NULL;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;

  pipelineLayoutInfo.pushConstantRangeCount = 0;
  if (vkCreatePipelineLayout(m_logicalDevice->GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    throw std::runtime_error("VKDirLightPassPipeline::Init() - failed to create pipeline layout!");
  
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
    throw std::runtime_error("VKDirLightPassPipeline::Init() - failed to create graphics pipeline!");
  
}


