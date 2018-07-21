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
  virtual std::vector<SceneGlobalDataType> GetGlobalDataTypes() const {return {GLOBAL_PROJ_VIEW_MATRIX};}
  virtual std::vector<size_t> GetObjUboSizes() const{ return {sizeof(VertexObjectMatrices), sizeof(FragMaterialData) }; }
  virtual std::vector<size_t> GetGlobalUboSize() const{return {sizeof(VertexViewProjMatrices)}; }
};

class VKSkyboxPassPipeline : public VKPipeline
{
  public :
  VKSkyboxPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, 
                         const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, const unsigned int& a_subpassIndex, 
                         const glm::vec2& a_res, const glm::vec4& a_viewportSettings ); 
  virtual void Init();
  virtual std::vector<SceneGlobalDataType> GetGlobalDataTypes() const {return {GLOBAL_PROJ_VIEW_MATRIX};}
  virtual std::vector<size_t> GetObjUboSizes()() const{return { sizeof(VertexObjectMatrices) };}
  virtual std::vector<size_t> GetGlobalUboSize() const{return { sizeof(VertexViewProjMatrices) };}
};


class VKLightPassPipeline : public VKPipeline
{
  public :
  VKLightPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, 
                      const unsigned int& a_subpassIndex, const glm::vec2& a_res, const glm::vec4& a_viewportSettings ); 
  virtual void Init();
  virtual std::vector<SceneGlobalDataType> GetGlobalDataTypes() const {return {GLOBAL_PROJ_VIEW_MATRIX, GLOBAL_LIGHT_FRAG_DATA};}
  virtual std::vector<size_t> GetObjUboSizes() const{return { sizeof(VertexObjectMatrices), sizeof(FragPointLight) };}
  virtual std::vector<size_t> GetGlobalUboSize() const{return { sizeof(VertexViewProjMatrices), sizeof(FragLightGlobalVars) };}
};

class VKDirLightPassPipeline : public VKPipeline
{
  public :
  VKDirLightPassPipeline(const std::shared_ptr<VulkanLogicalDeviceManager>& a_logicalDevice, VkRenderPass a_renderPass, const std::vector<VkPipelineShaderStageCreateInfo>& a_shaders, 
                      const unsigned int& a_subpassIndex, const glm::vec2& a_res, const glm::vec4& a_viewportSettings ); 
  virtual void Init();
  virtual std::vector<SceneGlobalDataType> GetGlobalDataTypes() const {return {GLOBAL_LIGHT_FRAG_DATA};}
  virtual std::vector<size_t> GetObjUboSizes() const{return { sizeof(FragDirectionalLight) };}
  virtual std::vector<size_t> GetGlobalUboSize() const{return { sizeof(VertexViewProjMatrices), sizeof(FragLightGlobalVars) };}
};