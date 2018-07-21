#pragma once


#include <vector>
#include "Common/Shapes/ASkybox.h"
#include "Vulkan/Shapes/VKShape.h"
#include "Vulkan/Core/VulkanMemory.h"
/// This is a class for creating and rendering a skybox
/// Modified by Stavros Bizelis - for the Graphics Coursework n City, University of London
/// Stripped down of any texture information and hardcoded paths
class VKSkybox : public ASkybox, public VKShape
{
private:
  std::shared_ptr<VulkanMemory> m_memory;
  std::shared_ptr<VulkanMemoryChunk> m_vkVertices;
  std::shared_ptr<VulkanMemoryChunk> m_vkIndices;
  unsigned int m_indicesCount;
public:
	VKSkybox(const std::shared_ptr<VulkanMemory>& a_memory);
	~VKSkybox();
	 
  virtual unsigned int GetMeshesCount(){ return 1;}
  virtual std::shared_ptr<VulkanMemoryChunk> GetVertices(const unsigned int& a_index = 0){return m_vkVertices;}
  virtual std::shared_ptr<VulkanMemoryChunk> GetIndices(const unsigned int& a_index = 0){return m_vkIndices;}
  virtual unsigned int GetIndicesCount(const unsigned int& a_index = 0){return m_indicesCount;}


	virtual void Render();
  virtual void Create();
	virtual void Release();
};

