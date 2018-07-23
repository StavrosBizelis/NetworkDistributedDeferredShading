#pragma once


#include "Vulkan/Core/VulkanMemory.h"
#include <memory>

class VKATexture
{
  public:
  virtual std::shared_ptr<VulkanImageMemoryChunk> GetImage() = 0;
  virtual std::shared_ptr<VulkanSampler> GetSampler() = 0;
};