#include "Vulkan/RenderControl/Pipelines/VKPipeline.h"



VKPipeline::VKPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int& a_subpassIndex, const glm::vec2& a_res, const glm::vec4& a_viewportSettings )
  :m_logicalDevice(a_logicalDevice), m_renderPass(a_renderPass), m_viewportSettings(a_viewportSettings), m_subpassIndex(a_subpassIndex), m_shaders(a_shaders), m_res(a_res), m_pipelineLayout(NULL), m_descriptorSetLayout(NULL) {}
  
  
VKPipeline::~VKPipeline(){}


VkPipeline VKPipeline::GetPipelineHandle() const{ return m_graphicsPipeline; }
VkPipelineLayout VKPipeline::GetPipelineLayout() const{ return m_pipelineLayout; }
VkDescriptorSetLayout VKPipeline::GetDescriptorSetLayout() const{return m_descriptorSetLayout;}
std::vector<VkDescriptorSetLayoutBinding> VKPipeline::GetDescriptorSetLayoutBindings() const{return m_descLayoutBindings;}

void VKPipeline::Clear()
{
  if( m_pipelineLayout )
    vkDestroyPipelineLayout(m_logicalDevice->GetDevice(), m_pipelineLayout, nullptr);
}

