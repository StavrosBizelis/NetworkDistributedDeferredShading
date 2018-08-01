#pragma once

#include <Vulkan/Vulkan.hpp>
#include <memory>

#include "Common/Shapes/IMesh.h"
#include "Vulkan/Core/VulkanMemory.h"
#include "Vulkan/Textures/VKATexture.h"
#include "Vulkan/Shapes/VKShape.h"
#include "Vulkan/RenderControl/UniformBufferObjects.h"
#include "glm/glm.hpp"

#include <iostream>

class VulkanRenderable
{
  protected:
  std::shared_ptr< std::vector<VulkanRenderable*> > m_updateRegistry;

  std::shared_ptr<VulkanMemoryChunk> m_uniformBuffer;
  std::shared_ptr<VulkanMemoryChunk> m_uniformBuffer2;
  VkDescriptorSet m_descSet;
  
  std::shared_ptr<IMesh> m_shape;
  
  public:
  VulkanRenderable(std::shared_ptr<IMesh> a_shape, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry);
  
  ~VulkanRenderable();
  
  virtual void Init(const VkDescriptorSet& a_descSet, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer1, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer2 );
  
  // needed for the actual command recording
  virtual std::shared_ptr<IMesh> GetShape();
  virtual void SetDesciptorSet(const VkDescriptorSet& a_descSet); ///< pointer to a descriptor set because it can be non existant
  virtual VkDescriptorSet* GetDesciptorSet(); ///< pointer to a descriptor set because it can be non existant
  
  
  void RegisterForUpdate();
  
  std::shared_ptr< std::vector<VulkanRenderable*> > GetUpdateRegistry();
  
  virtual void VulkanUpdate(char* a_mappedUBO) = 0;
};


class VulkanMeshRenderable : public VulkanRenderable
{
  public :
  glm::mat4 m_lastAbsoluteTrans;
  FragMaterialData m_ubo2;
  
  VulkanMeshRenderable(std::shared_ptr<IMesh> a_shape, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry);
    
  void VulkanUpdate(char* a_mappedUBO);
};


class VulkanDirLightRenderable : public VulkanRenderable
{
  public :
  VertexObjectMatrices m_ubo;
  FragDirectionalLight m_ubo2;
  
  VulkanDirLightRenderable(std::shared_ptr<IMesh> a_shape, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry);
    
  void VulkanUpdate(char* a_mappedUBO);
};

class VulkanPointLightRenderable : public VulkanRenderable
{
  public :
  VertexObjectMatrices m_ubo;
  FragPointLight m_ubo2;
  
  VulkanPointLightRenderable(std::shared_ptr<IMesh> a_shape, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry);
    
  void VulkanUpdate(char* a_mappedUBO);
};

class VulkanSpotLightRenderable : public VulkanRenderable
{
  public :
  VertexObjectMatrices m_ubo;
  FragSpotLight m_ubo2;
  
  VulkanSpotLightRenderable(std::shared_ptr<IMesh> a_shape, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry);
    
  void VulkanUpdate(char* a_mappedUBO);
};
