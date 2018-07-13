#pragma once

#include "Common/Shapes/ACube.h"
#include "Vulkan/Core/VulkanMemory.h"

/// a cube primitive mesh 
/// only vertex positions are set(no normals or texture coordinates)
/// used as a lighting mesh scene node
class VKCube : public ACube
{
private:
  std::shared_ptr<VulkanMemory> m_memory;
  std::shared_ptr<VulkanMemoryChunk> m_vkVertices;
  std::shared_ptr<VulkanMemoryChunk> m_vkIndices;
public:
	VKCube(const std::shared_ptr<VulkanMemory>& a_memory);
	~VKCube();
	
	virtual void Render();
	virtual void Release();
  virtual void Create();
};

