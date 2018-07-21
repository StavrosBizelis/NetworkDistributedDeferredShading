#pragma once
#include <memory>
#include "Vulkan/Core/VulkanMemory.h"

class VKShape
{
  public:
  virtual unsigned int GetMeshesCount() = 0;
  virtual std::shared_ptr<VulkanMemoryChunk> GetVertices(const unsigned int& a_index = 0) = 0;
  virtual std::shared_ptr<VulkanMemoryChunk> GetIndices(const unsigned int& a_index = 0) =0;
  virtual unsigned int GetIndicesCount(const unsigned int& a_index = 0) = 0;
};