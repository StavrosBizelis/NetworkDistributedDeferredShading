#pragma once

#include "Common/Shapes/ACone.h"
#include "Vulkan/Shapes/VKShape.h"
#include "Vulkan/Core/VulkanMemory.h"
/// a cone primitive mesh 
/// only vertex positions are set(no normals or texture coordinates)
/// used as a lighting mesh scene node
class VKCone : public ACone, public VKShape
{
private:
  std::shared_ptr<VulkanMemory> m_memory;
    std::shared_ptr<VulkanMemoryChunk> m_vkVertices;
  std::shared_ptr<VulkanMemoryChunk> m_vkIndices;
  
public:
	VKCone(const std::shared_ptr<VulkanMemory>& a_memory);
	~VKCone();
	
  virtual unsigned int GetMeshesCount(){ return 1;}
  virtual std::shared_ptr<VulkanMemoryChunk> GetVertices(const unsigned int& a_index = 0){return m_vkVertices;}
  virtual std::shared_ptr<VulkanMemoryChunk> GetIndices(const unsigned int& a_index = 0){return m_vkIndices;}
  virtual unsigned int GetIndicesCount(const unsigned int& a_index = 0){return m_indices.size();}


	virtual void Render();
	virtual void Release();
  virtual void Create();
};

