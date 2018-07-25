#pragma once


#include "Common/Shapes/ASphere.h"
#include "Vulkan/Shapes/VKShape.h"
#include "Vulkan/Core/VulkanMemory.h"

// Class for generating a unit sphere 
class VKSphere : public ASphere
{
private:
  VKShape* m_vulkanShape;
  std::shared_ptr<VulkanMemory> m_memory;
public:
	VKSphere(const std::shared_ptr<VulkanMemory>& a_memory);
	~VKSphere();
	


	virtual void Render();
	virtual void Release();
  virtual void Create();
  
  virtual void* GetExtra() {return reinterpret_cast<void*>(m_vulkanShape);}

};

