#pragma once

#include "Common/Shapes/ACone.h"
#include "Vulkan/Shapes/VKShape.h"
#include "Vulkan/Core/VulkanMemory.h"
/// a cone primitive mesh 
/// only vertex positions are set(no normals or texture coordinates)
/// used as a lighting mesh scene node
class VKCone : public ACone
{
private:
  
  VKShape* m_vulkanShape;
  std::shared_ptr<VulkanMemory> m_memory;
public:
	VKCone(const std::shared_ptr<VulkanMemory>& a_memory);
	~VKCone();
	
  virtual void Render();
	virtual void Release();
  virtual void Create();
  
  virtual void* GetExtra() {return reinterpret_cast<void*>(m_vulkanShape);}

};

