#include "Vulkan/RenderControl/Pipelines/VKPipeline.h"



VKPipeline::VKPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int& a_subpassIndex, const glm::vec2& a_res, const glm::vec4& a_viewportSettings )
  :m_logicalDevice(a_logicalDevice), m_renderPass(a_renderPass), m_viewportSettings(a_viewportSettings), m_subpassIndex(a_subpassIndex), m_shaders(a_shaders), m_res(a_res), m_pipelineLayout(NULL), m_descriptorSetLayout(NULL) {}
  
  
VKPipeline::~VKPipeline(){}


VkPipeline VKPipeline::GetPipelineHandle() const{ return m_graphicsPipeline; }
VkPipelineLayout VKPipeline::GetPipelineLayout() const{ return m_pipelineLayout; }
VkDescriptorSetLayout VKPipeline::GetDescriptorSetLayout() const{return m_descriptorSetLayout;}
std::vector<VkDescriptorSetLayoutBinding> VKPipeline::GetDescriptorSetLayoutBindings() const{return m_descLayoutBindings;}




bool VKPipeline::IsDirty(bool a_clean)
{
  bool l_tmp = m_dirty;
  m_dirty = a_clean ? false : m_dirty;
  return l_tmp;
}


void VKPipeline::AddSecondaryBuffer(const std::shared_ptr<VulkanSecondaryCommandBuffer>& a_buffer){  m_commandBuffers.push_back(a_buffer); m_dirty = true;  }
  
std::vector< std::shared_ptr<VulkanSecondaryCommandBuffer> > VKPipeline::GetSecondaryCommandBuffers() { return m_commandBuffers; }

std::vector< VkCommandBuffer> VKPipeline::GetSecondaryCommandBuffersHandles()
{
  std::vector< VkCommandBuffer> l_toReturn;
  // push all the objects' secondary buffers
  for( unsigned int i = 0; i < m_commandBuffers.size(); ++i)
  { 
    if(!m_commandBuffers[i]->IsReady() )
      m_commandBuffers[i]->RecordCommands(m_graphicsPipeline);
    l_toReturn.push_back( m_commandBuffers[i]->GetCommandBufferHandle() );
  }
  return l_toReturn;
}



void VKPipeline::Clear()
{
  if( m_pipelineLayout )
    vkDestroyPipelineLayout(m_logicalDevice->GetDevice(), m_pipelineLayout, nullptr);
}

