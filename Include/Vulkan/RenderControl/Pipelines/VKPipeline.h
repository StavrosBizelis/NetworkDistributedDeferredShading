#pragma once


#include "Vulkan/Core/VulkanMemory.h"
#include "Vulkan/Core/VulkanLogicalDeviceManager.h"
#include <Vulkan/RenderControl/UniformBufferObjects.h>
#include <Vulkan/RenderControl/Pipelines/VulkanSecondaryCommandBuffer.h>
#include <Vulkan/Vulkan.hpp>
#include <memory>
#include <glm/glm.hpp>

enum SceneGlobalDataType
{
  NO_GLOBAL_DATA = 0,
  GLOBAL_PROJ_VIEW_MATRIX,
  GLOBAL_LIGHT_FRAG_DATA,
};

class VKPipeline
{
  protected:
  std::shared_ptr<VulkanLogicalDeviceManager> m_logicalDevice;
    
  glm::vec4 m_viewportSettings;
  glm::vec2 m_res;
  const std::vector<VkPipelineShaderStageCreateInfo> m_shaders;
  

  VkDescriptorSetLayout m_descriptorSetLayout;
  // VkDescriptorPool m_descriptorPool;
  // VkDescriptorSet m_descriptorSet;
  
  
  VkRenderPass m_renderPass;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;
  unsigned int m_subpassIndex;
  
  std::vector< std::shared_ptr<VulkanSecondaryCommandBuffer> > m_commandBuffers;
  bool m_dirty;
  
  

  std::vector<VkDescriptorSetLayoutBinding> m_descLayoutBindings;
  public:
  VKPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int& a_subpassIndex, const glm::vec2& a_res, const glm::vec4& a_viewportSettings );
  virtual ~VKPipeline();
  
  
  virtual void Init() = 0;
  // void ReInit();    ///< reinitialization - does not delete the internal objects - just reinitializes them
  
  
  // IMPLEMENT THESE
  bool IsDirty(bool a_clean); ///< return if changes have been made. a_clean - if true, the pipeline changes are supposed to be taken into consideration
  void AddSecondaryBuffer(const std::shared_ptr<VulkanSecondaryCommandBuffer>& a_buffer);
  std::vector< std::shared_ptr<VulkanSecondaryCommandBuffer> > GetSecondaryCommandBuffers();
  std::vector< VkCommandBuffer> GetSecondaryCommandBuffersHandles();    ///< returns the secondary command buffers ( +the first one is a plus - and is the one that binds the pipeline to the render pass)

  VkPipeline GetPipelineHandle() const;
  VkPipelineLayout GetPipelineLayout() const;
  VkDescriptorSetLayout GetDescriptorSetLayout() const;
  
  std::vector<VkDescriptorSetLayoutBinding> GetDescriptorSetLayoutBindings() const;
    
  virtual std::vector<SceneGlobalDataType> GetGlobalDataTypes() const {return {NO_GLOBAL_DATA}; }
  virtual std::vector<size_t> GetGlobalUboSize() const{return {0};}
  virtual std::vector<size_t> GetObjUboSizes() const{return {0};}
  
  void Clear();
};