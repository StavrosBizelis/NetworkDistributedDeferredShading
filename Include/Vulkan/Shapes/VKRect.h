#pragma once


#include <vector>
#include "Common/Shapes/ARect.h"
#include "Vulkan/Core/VulkanMemory.h"

/// Used for screen space quads
/// originally it is full screen but with scaling it can be used for gui elements
class VKRect : public ARect
{
private:
  std::shared_ptr<VulkanMemory> m_memory;
  std::shared_ptr<VulkanMemoryChunk> m_vkVertices;
  std::shared_ptr<VulkanMemoryChunk> m_vkIndices;
public:
	VKRect(const std::shared_ptr<VulkanMemory>& a_memory);
	~VKRect();
	

	virtual void Render();
	virtual void Release();  
  virtual void Create();
};

