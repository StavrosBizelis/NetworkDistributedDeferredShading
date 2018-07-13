#pragma once


#include "Common/Shapes/ASphere.h"
#include "Vulkan/Core/VulkanMemory.h"

// Class for generating a unit sphere 
class VKSphere : public ASphere
{
private:
  std::shared_ptr<VulkanMemory> m_memory;
  std::shared_ptr<VulkanMemoryChunk> m_vkVertices;
  std::shared_ptr<VulkanMemoryChunk> m_vkIndices;
public:
	VKSphere(const std::shared_ptr<VulkanMemory>& a_memory);
	~VKSphere();
	

	virtual void Render();
	virtual void Release();
  virtual void Create();
};

