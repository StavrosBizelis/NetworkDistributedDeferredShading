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
  virtual SceneGlobalDataType GetGlobalDataTypes() const {return GLOBAL_PROJ_MATRIX;}
  virtual size_t GetObjUboSize() const{ return sizeof(VertexObjectMatrices); }
  virtual size_t GetGlobalUboSize() const{return sizeof(VertexSingleMat4); }
};

class VKSkyboxPassPipeline : public VKPipeline
{
  public :
  VKSkyboxPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, 
                         const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int& a_subpassIndex, 
                         const glm::vec2& a_res, const glm::vec4& a_viewportSettings ); 
  virtual void Init();
  virtual SceneGlobalDataType GetGlobalDataTypes() const {return NO_GLOBAL_DATA;}
  virtual size_t GetObjUboSize() const{return 0;}
  virtual size_t GetGlobalUboSize() const{return 0;}
};


class VKLightPassPipeline : public VKPipeline
{
  public :
  VKLightPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, 
                      const unsigned int& a_subpassIndex, const glm::vec2& a_res, const glm::vec4& a_viewportSettings ); 
  virtual void Init();
  virtual SceneGlobalDataType GetGlobalDataTypes() const {return NO_GLOBAL_DATA;}
  virtual size_t GetObjUboSize() const{return 0;}
  virtual size_t GetGlobalUboSize() const{return 0;}
};

class VKDirLightPassPipeline : public VKPipeline
{
  public :
  VKDirLightPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, 
                      const unsigned int& a_subpassIndex, const glm::vec2& a_res, const glm::vec4& a_viewportSettings ); 
  virtual void Init();
  virtual SceneGlobalDataType GetGlobalDataTypes() const {return NO_GLOBAL_DATA;}
  virtual size_t GetObjUboSize() const{return 0;}
  virtual size_t GetGlobalUboSize() const{return 0;}
};