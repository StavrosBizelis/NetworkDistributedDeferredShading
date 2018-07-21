#pragma once

#include "Common/Shapes/ACube.h"
#include "Vulkan/Shapes/VKShape.h"
#include "Vulkan/Core/VulkanMemory.h"

/// a cube primitive mesh 
/// only vertex positions are set(no normals or texture coordinates)
/// used as a lighting mesh scene node
class VKCube : public ACube, public VKShape
{
private:
  std::shared_ptr<VulkanMemory> m_memory;
  std::shared_ptr<VulkanMemoryChunk> m_vkVertices;
  std::shared_ptr<VulkanMemoryChunk> m_vkIndices;
  unsigned int m_indicesCount;
public:
	VKCube(const std::shared_ptr<VulkanMemory>& a_memory);
	~VKCube();
	
  virtual unsigned int GetMeshesCount(){ return 1;}
  virtual std::shared_ptr<VulkanMemoryChunk> GetVertices(const unsigned int& a_index = 0){return m_vkVertices;}
  virtual std::shared_ptr<VulkanMemoryChunk> GetIndices(const unsigned int& a_index = 0){return m_vkIndices;}
  virtual unsigned int GetIndicesCount(const unsigned int& a_index = 0){return m_indicesCount;}
  
	virtual void Render();
	virtual void Release();
  virtual void Create();
};

