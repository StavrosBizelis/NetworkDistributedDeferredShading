#include <VulkanDriver/VulkanCommand.h>


VulkanIndexedMesh::VulkanIndexedMesh(){}
VulkanIndexedMesh::VulkanIndexedMesh(const std::shared_ptr<VulkanMemoryBuffer>& a_vertexBuffer, const std::shared_ptr<VulkanMemoryBuffer>& a_indexBuffer, const unsigned int& a_indicesCount)
:m_vertexBuffer(a_vertexBuffer), m_indexBuffer(a_indexBuffer), m_indicesCount(a_indicesCount){}



VulkanSecondaryCommandBuffer::VulkanSecondaryCommandBuffer(const VkDevice& a_logicalDevice, const VkCommandPool& a_cmdPool, 
                                                           const VkDescriptorSet& a_descriptorSet, const VkPipelineLayout& a_pipelineLayout,
                                                           const VkRenderPass& a_renderPass, const unsigned int& a_subpassIndex)
  : m_logicalDevice(a_logicalDevice), m_cmdPool(a_cmdPool), m_descriptorSet(a_descriptorSet), m_pipelineLayout(a_pipelineLayout), m_ready(false),
    m_renderPass(a_renderPass), m_subpassIndex(a_subpassIndex) {}

VulkanSecondaryCommandBuffer::~VulkanSecondaryCommandBuffer(){}


bool VulkanSecondaryCommandBuffer::operator==(const VulkanSecondaryCommandBuffer &a_other) const
{
  if ( m_logicalDevice != a_other.m_logicalDevice) return false;
  if ( m_cmdPool != a_other.m_cmdPool) return false;
  if ( m_descriptorSet != a_other.m_descriptorSet) return false;
  if ( m_pipelineLayout != a_other.m_pipelineLayout) return false;
  if ( m_secondaryCmdBuffer != a_other.m_secondaryCmdBuffer) return false;
  if ( m_objectsToDraw.size() != a_other.m_objectsToDraw.size()) return false;
  
  return true;
}
bool VulkanSecondaryCommandBuffer::operator!=(const VulkanSecondaryCommandBuffer &a_other) const { return !(*this == a_other); }

void VulkanSecondaryCommandBuffer::Init()
{
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = m_cmdPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  allocInfo.commandBufferCount = (uint32_t)1;

  
  if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &m_secondaryCmdBuffer) != VK_SUCCESS) 
    throw std::runtime_error("failed to allocate command buffer!");  
  
  m_ready = false;
}

bool VulkanSecondaryCommandBuffer::IsReady(){ return m_ready; }

bool VulkanSecondaryCommandBuffer::RecordCommands(const VkPipeline& a_pipeline)
{
  
  VkCommandBufferInheritanceInfo l_inheritanceInfo;
  l_inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
  l_inheritanceInfo.pNext = NULL;
  l_inheritanceInfo.renderPass = m_renderPass;
  l_inheritanceInfo.subpass = m_subpassIndex;
  // this is null although we could set a framebuffer - but that would require to record a secondary command buffer for each framebuffer
  l_inheritanceInfo.framebuffer = VK_NULL_HANDLE;   
  l_inheritanceInfo.occlusionQueryEnable = VK_TRUE;
  l_inheritanceInfo.queryFlags = VK_QUERY_CONTROL_PRECISE_BIT;
  l_inheritanceInfo.pipelineStatistics = VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT;
  
  
  m_ready = false;
  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
  beginInfo.pInheritanceInfo = &l_inheritanceInfo;
  
  vkBeginCommandBuffer(m_secondaryCmdBuffer, &beginInfo);

  vkCmdBindPipeline(m_secondaryCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, a_pipeline );
  // actual recording happening here
  for (std::map<unsigned int, VulkanIndexedMesh >::iterator it=m_objectsToDraw.begin(); it!=m_objectsToDraw.end(); ++it)
  {
    VkBuffer l_vertexBuffers[] = {it->second.m_vertexBuffer->m_buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(m_secondaryCmdBuffer, 0, 1, l_vertexBuffers, offsets);

    vkCmdBindIndexBuffer(m_secondaryCmdBuffer, it->second.m_indexBuffer->m_buffer, 0, VK_INDEX_TYPE_UINT16);
    
    vkCmdBindDescriptorSets(m_secondaryCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);
    
    vkCmdDrawIndexed(m_secondaryCmdBuffer, static_cast<uint32_t>(it->second.m_indicesCount), 1, 0, 0, 0);
    
  }
  
  if (vkEndCommandBuffer(m_secondaryCmdBuffer) == VK_SUCCESS)
    m_ready = true;
  
  return m_ready;
  
}


void VulkanSecondaryCommandBuffer::AddIndexedMesh(const unsigned int& a_id, const VulkanIndexedMesh& a_indexedMesh)
{
  if ( m_objectsToDraw.find(a_id) == m_objectsToDraw.end() )
  {
    m_objectsToDraw[a_id] = a_indexedMesh;
    m_ready = false;
  }
  else
    throw std::runtime_error("VulkanSecondaryCommandBuffer.AddIndexedMesh - id already exists");
}

void VulkanSecondaryCommandBuffer::RemoveIndexedMesh(const unsigned int& a_id)
{
  std::map<unsigned int, VulkanIndexedMesh >::iterator l_iter = m_objectsToDraw.find(a_id);
  if ( l_iter != m_objectsToDraw.end() )
  {
    m_objectsToDraw.erase(l_iter);
    m_ready = false;
  }
}


VkCommandBuffer VulkanSecondaryCommandBuffer::GetCommandBufferHandle(){ return m_secondaryCmdBuffer; }

void VulkanSecondaryCommandBuffer::Clear()
{
  vkFreeCommandBuffers(m_logicalDevice, m_cmdPool, static_cast<uint32_t>(1), &m_secondaryCmdBuffer);
}







VulkanPipelineOptions::VulkanPipelineOptions()
: m_blendingEnabled(false), m_depthTestEnabled(true), m_depthTestWrite(true), m_depthTestOperation(VK_COMPARE_OP_LESS),
  m_stencilTestEnabled(false), m_cullingMode(VK_CULL_MODE_BACK_BIT), m_polygonMode(VK_POLYGON_MODE_FILL),
  m_sourceBlendingFactor(VK_BLEND_FACTOR_ONE), m_sourceAlphaBlendingFactor(VK_BLEND_FACTOR_SRC_ALPHA), 
  m_destBlendingFactor(VK_BLEND_FACTOR_ZERO), m_destAlphaBlendingFactor(VK_BLEND_FACTOR_ONE),
  m_blendingOp(VK_BLEND_OP_ADD), m_blendingAlphaOp(VK_BLEND_OP_ADD ), m_shadersVars( std::make_shared<std::vector<ZRShaderVariable >>() )
  {}
  
VulkanPipelineOptions::VulkanPipelineOptions(const VulkanPipelineOptions& a_other)
{
  m_shadersVars = a_other.m_shadersVars;
  m_blendingEnabled = a_other.m_blendingEnabled;
  m_sourceBlendingFactor = a_other.m_sourceBlendingFactor;
  m_sourceAlphaBlendingFactor = a_other.m_sourceAlphaBlendingFactor;
  m_destBlendingFactor = a_other.m_destBlendingFactor;
  m_destAlphaBlendingFactor = a_other.m_destAlphaBlendingFactor;
  m_blendingOp = a_other.m_blendingOp;
  m_blendingAlphaOp = a_other.m_blendingAlphaOp;
  
  m_depthTestEnabled = a_other.m_depthTestEnabled;
  m_depthTestWrite = a_other.m_depthTestWrite;
  m_depthTestOperation = a_other.m_depthTestOperation;
 
  // stencil tests
  m_stencilTestEnabled = a_other.m_stencilTestEnabled;
  
  // stencil front face
  m_frontStencilFunc = a_other.m_frontStencilFunc;
  m_frontStencilReference = a_other.m_frontStencilReference;
  m_frontStencilMask = a_other.m_frontStencilMask;
  
  m_frontOnStencilFail = a_other.m_frontOnStencilFail;
  m_frontOnDepthFail = a_other.m_frontOnDepthFail;
  m_frontOnStencilDepthPass = a_other.m_frontOnStencilDepthPass;
  
  m_backStencilFunc = a_other.m_backStencilFunc;
  m_backStencilReference = a_other.m_backStencilReference;
  m_backStencilMask = a_other.m_backStencilMask;
  m_backOnStencilFail = a_other.m_backOnStencilFail;
  m_backOnDepthFail = a_other.m_backOnDepthFail;
  m_backOnStencilDepthPass = a_other.m_backOnStencilDepthPass;
  
  // rasterizer
  m_cullingMode = a_other.m_cullingMode;
  m_polygonMode = a_other.m_polygonMode;
}

VulkanPipelineOptions& VulkanPipelineOptions::operator=(const VulkanPipelineOptions& a_other)
{
  m_shadersVars = a_other.m_shadersVars;
  m_blendingEnabled = a_other.m_blendingEnabled;
  m_sourceBlendingFactor = a_other.m_sourceBlendingFactor;
  m_sourceAlphaBlendingFactor = a_other.m_sourceAlphaBlendingFactor;
  m_destBlendingFactor = a_other.m_destBlendingFactor;
  m_destAlphaBlendingFactor = a_other.m_destAlphaBlendingFactor;
  m_blendingOp = a_other.m_blendingOp;
  m_blendingAlphaOp = a_other.m_blendingAlphaOp;
  
  m_depthTestEnabled = a_other.m_depthTestEnabled;
  m_depthTestWrite = a_other.m_depthTestWrite;
  m_depthTestOperation = a_other.m_depthTestOperation;
 
  // stencil tests
  m_stencilTestEnabled = a_other.m_stencilTestEnabled;
  
  // stencil front face
  m_frontStencilFunc = a_other.m_frontStencilFunc;
  m_frontStencilReference = a_other.m_frontStencilReference;
  m_frontStencilMask = a_other.m_frontStencilMask;
  
  m_frontOnStencilFail = a_other.m_frontOnStencilFail;
  m_frontOnDepthFail = a_other.m_frontOnDepthFail;
  m_frontOnStencilDepthPass = a_other.m_frontOnStencilDepthPass;
  
  m_backStencilFunc = a_other.m_backStencilFunc;
  m_backStencilReference = a_other.m_backStencilReference;
  m_backStencilMask = a_other.m_backStencilMask;
  m_backOnStencilFail = a_other.m_backOnStencilFail;
  m_backOnDepthFail = a_other.m_backOnDepthFail;
  m_backOnStencilDepthPass = a_other.m_backOnStencilDepthPass;
  
  // rasterizer
  m_cullingMode = a_other.m_cullingMode;
  m_polygonMode = a_other.m_polygonMode;
  return *this;
}




VkShaderModule VulkanPipeline::CreateShaderModule(const std::vector<char>& a_code, const VkDevice& a_device)
{
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = a_code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(a_code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(a_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    throw std::runtime_error("failed to create shader module!");

  return shaderModule;
}

void VulkanPipeline::CreateDescriptorSetLayout()
{
  if( !m_options.m_shadersVars )
    throw std::runtime_error("failed to CreateDescriptorSetLayout - non existent m_shadersVars - USE SetShadersVariablesDescription");
  if( m_options.m_shadersVars->size() == 0 )
    throw std::runtime_error("failed to CreateDescriptorSetLayout - non existent m_shadersVars - USE SetShadersVariablesDescription");
  
  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(m_options.m_shadersVars->size());
  // create a vector of inputAttributes in order to create the descriptor set layout
  std::vector<VkDescriptorSetLayoutBinding> l_tmp;
  l_tmp.reserve( m_options.m_shadersVars->size() );
  for(unsigned int i = 0; i < m_options.m_shadersVars->size(); ++i)
    l_tmp.push_back(m_options.m_shadersVars->at(i).m_inputAttribute );
  
  layoutInfo.pBindings = l_tmp.data();

  if (vkCreateDescriptorSetLayout(m_logicalDevice, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
    throw std::runtime_error("failed to create descriptor set layout!");

}

void VulkanPipeline::CreateDescriptorPool()
{
  if( !m_options.m_shadersVars )
    throw std::runtime_error("failed to CreateDescriptorPool - non existent m_shadersVars - USE SetShadersVariablesDescription");
  if( m_options.m_shadersVars->size() == 0 )
    throw std::runtime_error("failed to CreateDescriptorPool - non existent m_shadersVars - USE SetShadersVariablesDescription");
  
  
  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(m_options.m_shadersVars->size());
  
  // create a vector of poolSize in order to create the descriptor pool
  std::vector<VkDescriptorPoolSize> l_tmp;
  l_tmp.reserve( m_options.m_shadersVars->size() );
  for(unsigned int i = 0; i < m_options.m_shadersVars->size(); ++i)
    l_tmp.push_back(m_options.m_shadersVars->at(i).m_poolSize );
  
  poolInfo.pPoolSizes = l_tmp.data();
  poolInfo.maxSets = 1;

  if (vkCreateDescriptorPool(m_logicalDevice, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
    throw std::runtime_error("failed to create descriptor pool!");
}

void VulkanPipeline::CreateDescriptorSet()
{
  VkDescriptorSetLayout layouts[] = {m_descriptorSetLayout};
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = m_descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = layouts;

  if (vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, &m_descriptorSet) != VK_SUCCESS)
      throw std::runtime_error("failed to allocate descriptor set!");
  
  std::vector<VkWriteDescriptorSet> l_descriptorWrites;
  for(unsigned int i = 0; i < m_options.m_shadersVars->size(); ++i)
  {
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = m_options.m_shadersVars->at(i).m_uniformBuffer.first->m_buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = m_options.m_shadersVars->at(i).m_uniformBuffer.second;

    VkWriteDescriptorSet l_tmp;
    l_tmp.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    l_tmp.pNext = NULL;
    l_tmp.dstSet = m_descriptorSet;
    l_tmp.dstBinding = 0;
    l_tmp.dstArrayElement = 0;
    l_tmp.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    l_tmp.descriptorCount = 1;
    l_tmp.pBufferInfo = &bufferInfo;
    
    l_descriptorWrites.push_back(l_tmp);
  }

  /// @TODO add functionality for textures too
  vkUpdateDescriptorSets(m_logicalDevice, static_cast<uint32_t>(l_descriptorWrites.size()), l_descriptorWrites.data(), 0, nullptr);
  
}


VulkanPipeline::VulkanPipeline(const VkDevice& a_logicalDevice, const VkCommandPool& a_cmdPool, const VkRenderPass& a_renderPass, const unsigned int& a_subpassIndex, 
                 const VkVertexInputBindingDescription& a_bindingDesc, const std::vector<VkVertexInputAttributeDescription>& a_attributeDesc,
                 const std::vector<char>& a_vertexCode, const std::vector<char>& a_fragCode, const VkExtent2D& a_extend )
  :m_logicalDevice(a_logicalDevice), m_cmdPool(a_cmdPool), m_renderPass(a_renderPass), m_subpassIndex(a_subpassIndex), 
   m_bindingDesc(a_bindingDesc), m_attributeDesc(std::make_shared<std::vector<VkVertexInputAttributeDescription>>(a_attributeDesc) ),
   m_vertexCode( std::make_shared<std::vector<char> >(a_vertexCode) ), m_fragCode( std::make_shared<std::vector<char> >(a_fragCode) ),
   m_commandBuffers(std::make_shared<std::vector< std::shared_ptr<VulkanSecondaryCommandBuffer> > >() ), m_extend(a_extend), m_dirty(true){}


VulkanPipeline::~VulkanPipeline(){}

bool VulkanPipeline::operator==(const VulkanPipeline &a_other) const
{
  if ( m_logicalDevice != a_other.m_logicalDevice) return false;
  // if ( m_extend != a_other.m_extend) return false;
  if ( m_graphicsPipeline != a_other.m_graphicsPipeline) return false;
  if ( m_cmdPool != a_other.m_cmdPool) return false;
  if ( m_secondaryCmdBuffer != a_other.m_secondaryCmdBuffer) return false;

  if( m_commandBuffers != NULL && a_other.m_commandBuffers == NULL) return false;
  if( m_commandBuffers == NULL && a_other.m_commandBuffers != NULL) return false;
  if( m_commandBuffers != NULL && a_other.m_commandBuffers != NULL)
  {
    if( m_commandBuffers->size() != a_other.m_commandBuffers->size() ) return false;
    for( unsigned int i = 0; i < m_commandBuffers->size(); ++i )
      if( m_commandBuffers->at(i) != a_other.m_commandBuffers->at(i) ) return false;
  }
  
  return true;
}

bool VulkanPipeline::operator!=(const VulkanPipeline &a_other) const { return !(*this == a_other); }

void VulkanPipeline::Init()
{
  VkShaderModule vertShaderModule = CreateShaderModule(*m_vertexCode, m_logicalDevice);
  VkShaderModule fragShaderModule = CreateShaderModule(*m_fragCode, m_logicalDevice);
  
  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";
  
  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";
  
  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
  
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attributeDesc->size());
  vertexInputInfo.pVertexBindingDescriptions = &m_bindingDesc;
  vertexInputInfo.pVertexAttributeDescriptions = m_attributeDesc->data();
  
  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;
  
  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float) m_extend.width;
  viewport.height = (float) m_extend.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = {0, 0};
  scissor.extent = m_extend;
  
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
  rasterizer.polygonMode = (VkPolygonMode)m_options.m_polygonMode;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = (VkCullModeFlags)m_options.m_cullingMode;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  
  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  
  VkPipelineDepthStencilStateCreateInfo depthStencil = {};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = m_options.m_depthTestEnabled ? VK_TRUE : VK_FALSE;
  depthStencil.depthWriteEnable = m_options.m_depthTestWrite ? VK_TRUE : VK_FALSE;
  depthStencil.depthCompareOp = VkCompareOp(m_options.m_depthTestOperation);
  
  depthStencil.stencilTestEnable = m_options.m_stencilTestEnabled ? VK_TRUE : VK_FALSE;
  
  VkStencilOpState l_front;
  l_front.failOp = VkStencilOp(m_options.m_frontOnStencilFail);
  l_front.passOp = VkStencilOp(m_options.m_frontOnStencilDepthPass);
  l_front.depthFailOp = VkStencilOp(m_options.m_frontOnDepthFail);
  l_front.compareOp = VkCompareOp(m_options.m_frontStencilFunc);
  l_front.compareMask = uint32_t(m_options.m_frontStencilMask);
  l_front.writeMask = uint32_t(m_options.m_frontStencilMask);
  l_front.reference = uint32_t(m_options.m_frontStencilReference);

  VkStencilOpState l_back;
  l_back.failOp = VkStencilOp(m_options.m_backOnStencilFail);
  l_back.passOp = VkStencilOp(m_options.m_backOnStencilDepthPass);
  l_back.depthFailOp = VkStencilOp(m_options.m_backOnDepthFail);
  l_back.compareOp = VkCompareOp(m_options.m_backStencilFunc);
  l_back.compareMask = uint32_t(m_options.m_backStencilMask);
  l_back.writeMask = uint32_t(m_options.m_backStencilMask);
  l_back.reference = uint32_t(m_options.m_backStencilReference);
  
  depthStencil.front = l_front;
  depthStencil.back = l_back;
  
  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = m_options.m_blendingEnabled ? VK_TRUE : VK_FALSE;;
  colorBlendAttachment.srcColorBlendFactor = VkBlendFactor(m_options.m_sourceBlendingFactor); // Optional
  colorBlendAttachment.dstColorBlendFactor = VkBlendFactor(m_options.m_destBlendingFactor); // Optional
  colorBlendAttachment.colorBlendOp = VkBlendOp(m_options.m_blendingOp); // Optional
  colorBlendAttachment.srcAlphaBlendFactor = VkBlendFactor(m_options.m_sourceAlphaBlendingFactor); // Optional
  colorBlendAttachment.dstAlphaBlendFactor = VkBlendFactor(m_options.m_destAlphaBlendingFactor); // Optional
  colorBlendAttachment.alphaBlendOp = VkBlendOp(m_options.m_blendingAlphaOp); // Optional
  
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
  
  
  
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  if( m_options.m_shadersVars )
    if( m_options.m_shadersVars->size() > 0 )
    {
      CreateDescriptorSetLayout();
      pipelineLayoutInfo.setLayoutCount = 1;
      pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
    }
    
    
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  
  if (vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    throw std::runtime_error("failed to create pipeline layout!");
  
  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
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
  
  
  if (vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline )  != VK_SUCCESS)
    throw std::runtime_error("failed to create graphics pipeline!");

  vkDestroyShaderModule(m_logicalDevice, fragShaderModule, nullptr);
  vkDestroyShaderModule(m_logicalDevice, vertShaderModule, nullptr);
  
  // allocate the secondary command buffer that is supposed to bind the pipeline
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = m_cmdPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  allocInfo.commandBufferCount = (uint32_t)1;

  
  if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &m_secondaryCmdBuffer) != VK_SUCCESS) 
    throw std::runtime_error("VulkanPipeline - failed to allocate command buffer!");
  
  
  CreateDescriptorPool();
  CreateDescriptorSet();
}

void VulkanPipeline::ReInit()
{
  Init();
  for( std::vector< std::shared_ptr<VulkanSecondaryCommandBuffer> >::iterator l_iter = m_commandBuffers->begin(); l_iter != m_commandBuffers->end(); ++l_iter)
    (*l_iter)->Init();
  m_dirty = true;
}

bool VulkanPipeline::IsDirty(bool a_clean)
{
  bool l_tmp = m_dirty;
  m_dirty = a_clean ? false : m_dirty;
  return l_tmp;
}

void VulkanPipeline::AddSecondaryBuffer(const std::shared_ptr<VulkanSecondaryCommandBuffer>& a_buffer){  m_commandBuffers->push_back(a_buffer); m_dirty = true;  }
  
std::shared_ptr<std::vector< std::shared_ptr<VulkanSecondaryCommandBuffer> > > VulkanPipeline::GetSecondaryCommandBuffers() { return m_commandBuffers; }

std::shared_ptr<std::vector< VkCommandBuffer> > VulkanPipeline::GetSecondaryCommandBuffersHandles()
{
 
  std::shared_ptr<std::vector< VkCommandBuffer> > l_toReturn( std::make_shared<std::vector< VkCommandBuffer>>( std::vector< VkCommandBuffer>() ) );
  // push all the objects' secondary buffers
  for( unsigned int i = 0; i < m_commandBuffers->size(); ++i)
    if(m_commandBuffers->at(i)->IsReady() )
      l_toReturn->push_back( m_commandBuffers->at(i)->GetCommandBufferHandle() );
    else if( m_commandBuffers->at(i)->RecordCommands(m_graphicsPipeline) )
      l_toReturn->push_back( m_commandBuffers->at(i)->GetCommandBufferHandle() );
    
  return l_toReturn;
}

VkPipeline VulkanPipeline::GetPipelineHandle(){ return m_graphicsPipeline; } 
VkPipelineLayout VulkanPipeline::GetPipelineLayout(){ return m_pipelineLayout; }
VkDescriptorSetLayout VulkanPipeline::GetDescriptorSetLayout(){ return m_descriptorSetLayout; } 
VkDescriptorPool VulkanPipeline::GetDescriptorPool(){ return m_descriptorPool; } 
VkDescriptorSet VulkanPipeline::GetDescriptorSet(){ return m_descriptorSet; }

void VulkanPipeline::SetOptions(const VulkanPipelineOptions& a_options)
{
  m_options = a_options; 
  Init();
  m_dirty = true;
}

VulkanPipelineOptions VulkanPipeline::GetOptions(){ return m_options; }

void VulkanPipeline::Clear()
{
  vkDestroyPipeline(m_logicalDevice, m_graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(m_logicalDevice, m_pipelineLayout, nullptr);
  if( m_options.m_shadersVars )
    if( m_options.m_shadersVars->size() > 0 )
      vkDestroyDescriptorSetLayout(m_logicalDevice, m_descriptorSetLayout, nullptr);
}















void VulkanPrimaryCommandBuffer::RecordCommands()
{
  for (size_t i = 0; i < m_cmdBuffers->size(); i++) 
  {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(m_cmdBuffers->at(i), &beginInfo);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = m_swapChainFramebuffers->at(i);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapChainExtent;

    
    renderPassInfo.clearValueCount = static_cast<uint32_t>(m_clearValues.size());
    renderPassInfo.pClearValues = m_clearValues.data();
    //begin render pass
    vkCmdBeginRenderPass(m_cmdBuffers->at(i), &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

    
    // actual recording happening here
    std::queue<std::shared_ptr< VulkanPipeline > > l_newPipelines;
    unsigned int l_counter = m_pipelines.size();
    while( !m_pipelines.empty() )
    {
      l_counter--;
      std::shared_ptr< VulkanPipeline >  l_currPipeline = m_pipelines.front();
      m_pipelines.pop();
      
      
      
      this->RecordPipeline( m_cmdBuffers->at(i), l_currPipeline );
      l_newPipelines.push(l_currPipeline);
      // go to next subpass - in order to attach the next pipeline
      if( l_counter > 0 )
        vkCmdNextSubpass(m_cmdBuffers->at(i), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
    }
    m_pipelines = std::move( l_newPipelines );
    
    
    // end render pass
    vkCmdEndRenderPass(m_cmdBuffers->at(i));

    if (vkEndCommandBuffer(m_cmdBuffers->at(i) ) != VK_SUCCESS)
      throw std::runtime_error("VulkanPrimaryCommandBuffer - failed to record primary command buffer!");
  }
}

void VulkanPrimaryCommandBuffer::RecordPipeline(VkCommandBuffer a_cmdBuffer, const std::shared_ptr< VulkanPipeline >& a_pipeline)
{
    // record pipelines(each pipeline is responsible to provide a recorded secondary command buffer that binds the pipeline to the renderpass)
    std::shared_ptr<std::vector< VkCommandBuffer> > l_secondaryCmdBuffs = a_pipeline->GetSecondaryCommandBuffersHandles();
    if( l_secondaryCmdBuffs )
      if( !l_secondaryCmdBuffs->empty() )
      {
        // executing the recorded commands from the secondary command buffers
        // vkCmdExecuteCommands(a_cmdBuffer, (uint32_t)1, l_secondaryCmdBuffs->data() );
        vkCmdExecuteCommands(a_cmdBuffer, (uint32_t)l_secondaryCmdBuffs->size(), l_secondaryCmdBuffs->data() );
      }
}
  
VulkanPrimaryCommandBuffer::VulkanPrimaryCommandBuffer(){}

VulkanPrimaryCommandBuffer::VulkanPrimaryCommandBuffer(const VkDevice& a_logicalDevice, const VkCommandPool& a_cmdPool,
                                                       const std::shared_ptr< std::vector<VkFramebuffer> >& a_swapChainFramebuffers, 
                                                       const VkRenderPass& a_renderPass, 
                                                       const VkExtent2D& a_swapChainExtent  )
  : m_logicalDevice(a_logicalDevice), m_cmdPool(a_cmdPool), m_swapChainFramebuffers(a_swapChainFramebuffers), m_renderPass(a_renderPass), m_swapChainExtent(a_swapChainExtent), 
    m_cmdBuffers( std::make_shared<std::vector<VkCommandBuffer>>( std::vector<VkCommandBuffer>() ) ), m_clearValues({}), m_index(0), m_dirty(true)
{
  m_clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
  m_clearValues[1].depthStencil = {1.0f, 0};
}
                            
VulkanPrimaryCommandBuffer::~VulkanPrimaryCommandBuffer() {}


void VulkanPrimaryCommandBuffer::Init()
{
  if( !m_swapChainFramebuffers) 
    throw std::runtime_error("VulkanPrimaryCommandBuffer - failed to allocate command buffers! - no swapchain frame buffers were provided");
  
  
  m_cmdBuffers->resize( m_swapChainFramebuffers->size() );

  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = m_cmdPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t) m_cmdBuffers->size();

  if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, m_cmdBuffers->data()) != VK_SUCCESS)
      throw std::runtime_error("VulkanPrimaryCommandBuffer - failed to allocate command buffers! - vkAllocateCommandBuffers failed");
  m_dirty = true;
}

void VulkanPrimaryCommandBuffer::ReInit()
{
  // initialize primary command buffer
  Init();
  
  // reinitialize secondary command buffers
  std::queue<std::shared_ptr< VulkanPipeline > > l_newPipelines;

  
  // reinitialize pipelines
  while( !m_pipelines.empty() )
  {
    std::shared_ptr< VulkanPipeline >  l_currPipeline = m_pipelines.front();

    // reinitialize the current pipeline
    l_currPipeline->ReInit();
    m_pipelines.pop();
    l_newPipelines.push(l_currPipeline);
  }
  m_pipelines = std::move( l_newPipelines );
  m_dirty = true;
}

void VulkanPrimaryCommandBuffer::AddPipeline(const std::shared_ptr< VulkanPipeline >& a_pipeline) { m_pipelines.push(a_pipeline); m_dirty = true;}
  
void VulkanPrimaryCommandBuffer::RemovePipeline(const std::shared_ptr< VulkanPipeline >& a_pipeline)
{
  std::queue<std::shared_ptr< VulkanPipeline > > l_newPipelines;
  while( !m_pipelines.empty() )
  {
    std::shared_ptr< VulkanPipeline >  l_currPipeline = m_pipelines.front();
    m_pipelines.pop();
    
    // push it in the temporary queue only if it is not the given pipeline
    if( !(a_pipeline == l_currPipeline) ) 
      l_newPipelines.push(l_currPipeline);
  } 
  m_pipelines = std::move( l_newPipelines );
  m_dirty = true;
}
  

void VulkanPrimaryCommandBuffer::Update()
{
  std::queue<std::shared_ptr< VulkanPipeline > > l_newPipelines;
  while( !m_pipelines.empty() )
  {
    std::shared_ptr< VulkanPipeline > l_currPipeline = m_pipelines.front();
    
    // check for anything changed
    if( l_currPipeline->IsDirty(true) )
      m_dirty = true;
    
    m_pipelines.pop();
    l_newPipelines.push(l_currPipeline);
  }
  m_pipelines = std::move( l_newPipelines );
  
  
  if( m_dirty )
  {
    RecordCommands();
  }
  
  m_dirty = false;
}


VkCommandBuffer VulkanPrimaryCommandBuffer::GetNextCommandBufferHandle()
{
  // increment command buffer index and check if out of bounds
  m_index++;
  if( m_index >= m_cmdBuffers->size() )
    m_index = 0;
  
  // get the next command buffer
  VkCommandBuffer l_nextCmdBuffer = m_cmdBuffers->at(m_index);
  return l_nextCmdBuffer;
}

std::queue<std::shared_ptr< VulkanPipeline > > VulkanPrimaryCommandBuffer::GetPipelines(){ return m_pipelines; }

void VulkanPrimaryCommandBuffer::Clear(){ vkFreeCommandBuffers(m_logicalDevice, m_cmdPool, static_cast<uint32_t>(m_cmdBuffers->size()), m_cmdBuffers->data() );  }

