#pragma once

#include "Vulkan/RenderControl/Pipelines/VKPipeline.h"


class VKGeometryPassPipeline : public VKPipeline
{
  unsigned int m_inSamplersCount;
  public :
  VKGeometryPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, 
                         const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int& a_subpassIndex, 
                         const glm::vec2& a_res, const glm::vec4& a_viewportSettings, const unsigned int& a_inSamplersCount ); 
  virtual void Init();
};

class VKSkyboxPassPipeline : public VKPipeline
{
  public :
  VKSkyboxPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, 
                         const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int& a_subpassIndex, 
                         const glm::vec2& a_res, const glm::vec4& a_viewportSettings ); 
  virtual void Init();
};


class VKLightPassPipeline : public VKPipeline
{
  public :
  VKLightPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, 
                      const unsigned int& a_subpassIndex, const glm::vec2& a_res, const glm::vec4& a_viewportSettings ); 
  virtual void Init();
};

class VKDirLightPassPipeline : public VKPipeline
{
  public :
  VKDirLightPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, 
                      const unsigned int& a_subpassIndex, const glm::vec2& a_res, const glm::vec4& a_viewportSettings ); 
  virtual void Init();
};