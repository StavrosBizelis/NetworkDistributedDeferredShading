#pragma once

#include "Common/Shapes/ACone.h"
#include "Vulkan/Core/VulkanMemory.h"
/// a cone primitive mesh 
/// only vertex positions are set(no normals or texture coordinates)
/// used as a lighting mesh scene node
class VKCone : public ACone
{
private:
  std::shared_ptr<VulkanMemory> m_memory;
    std::shared_ptr<VulkanMemoryChunk> m_vkVertices;
  std::shared_ptr<VulkanMemoryChunk> m_vkIndices;
  
public:
	VKCone(const std::shared_ptr<VulkanMemory>& a_memory);
	~VKCone();
	

	virtual void Render();
	virtual void Release();
  virtual void Create();
};

