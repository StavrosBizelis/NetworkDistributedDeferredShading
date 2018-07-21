#pragma once

class VKATexture
{
  public:
  virtual std::shared_ptr<VulkanImageMemoryChunk> GetImage() = 0;
  virtual std::shared_ptr<VulkanSampler> GetSampler() = 0;
};