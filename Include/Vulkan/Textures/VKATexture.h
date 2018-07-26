#pragma once


#include "Vulkan/Core/VulkanMemory.h"
#include <memory>

class VKATexture
{
  protected:
  
  std::shared_ptr<VulkanMemory> m_memory;
  
  public:
  std::shared_ptr<VulkanImageMemoryChunk> m_image;
  std::shared_ptr<VulkanSampler> m_sampler;
  

  VKATexture(std::shared_ptr<VulkanMemory> a_memory, std::shared_ptr<VulkanImageMemoryChunk> a_image, std::shared_ptr<VulkanSampler> a_sampler)
    : m_memory(a_memory), m_image(a_image), m_sampler(a_sampler){}
    
  ~VKATexture()
  {
    if( m_image )
      m_image->Free();
  }
  std::shared_ptr<VulkanImageMemoryChunk> GetImage(){ return m_image; }
  std::shared_ptr<VulkanSampler> GetSampler(){ return m_sampler; }
};