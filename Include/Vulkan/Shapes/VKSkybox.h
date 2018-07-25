#pragma once


#include <vector>
#include "Common/Shapes/ASkybox.h"
#include "Vulkan/Shapes/VKShape.h"
#include "Vulkan/Core/VulkanMemory.h"
/// This is a class for creating and rendering a skybox
/// Modified by Stavros Bizelis - for the Graphics Coursework n City, University of London
/// Stripped down of any texture information and hardcoded paths
class VKSkybox : public ASkybox
{
private:

  
  VKShape* m_vulkanShape;
  std::shared_ptr<VulkanMemory> m_memory;
public:
	VKSkybox(const std::shared_ptr<VulkanMemory>& a_memory);
	~VKSkybox();
	 
	virtual void Render();
  virtual void Create();
	virtual void Release();
  
  virtual void* GetExtra() {return reinterpret_cast<void*>(m_vulkanShape);}

};

