#pragma once

#include <Vulkan/Vulkan.hpp>
#include <memory>
#include "Vulkan/Core/VulkanMemory.h"
#include "Vulkan/Textures/VKATexture.h"
#include "Vulkan/Shapes/VKShape.h"



class VulkanRenderable
{
  protected:
  std::shared_ptr< std::vector<VulkanRenderable*> > m_updateRegistry;
  public:
  VulkanRenderable(std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry)
  : m_updateRegistry(a_updateRegistry){}
  
  virtual void Init(const VkDescriptorSet& a_descSet, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer1, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer2 ) = 0;
  // needed for the actual command recording
  virtual std::shared_ptr<VKShape> GetShape() = 0;
  virtual void SetDesciptorSet(const VkDescriptorSet& a_descSet) = 0; ///< pointer to a descriptor set because it can be non existant
  virtual VkDescriptorSet* GetDesciptorSet() = 0; ///< pointer to a descriptor set because it can be non existant
  
  
  // needed for possible data updates
  virtual std::shared_ptr<VulkanMemoryChunk> GetUniformBuffer() = 0;
  virtual std::shared_ptr<VKATexture> GetVKTexture(const unsigned int& a_index) = 0;
  
  
  void RegisterForUpdate()
  {
    if( m_updateRegistry )
      m_updateRegistry->push_back(this);
  }
  std::shared_ptr< std::vector<VulkanRenderable*> > GetUpdateRegistry() {return m_updateRegistry; }
  
  virtual void VulkanUpdate(char* a_mappedUBO) = 0;
};