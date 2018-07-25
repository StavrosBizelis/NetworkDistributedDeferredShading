#pragma once


#include <vector>
#include "Common/Shapes/ARect.h"
#include "Vulkan/Shapes/VKShape.h"
#include "Vulkan/Core/VulkanMemory.h"

/// Used for screen space quads
/// originally it is full screen but with scaling it can be used for gui elements
class VKRect : public ARect
{
private:
  VKShape* m_vulkanShape;
  std::shared_ptr<VulkanMemory> m_memory;
public:
	VKRect(const std::shared_ptr<VulkanMemory>& a_memory);
	~VKRect();
	
	virtual void Render();
	virtual void Release();  
  virtual void Create();
  virtual void* GetExtra() {return reinterpret_cast<void*>(m_vulkanShape);}

};

