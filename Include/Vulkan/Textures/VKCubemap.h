#pragma once

#include "Common/Textures/ACubemap.h"
#include "Vulkan/Textures/VKATexture.h"
#include <string>
#include "Vulkan/Core/VulkanMemory.h"
#include <memory>
// Class that provides a texture for texture mapping
class VKCubemap : public ACubemap
{
  
  std::shared_ptr<VulkanMemory> m_memory;
  
  VKATexture* m_vkTextureData;
public:
  
  VKCubemap(std::shared_ptr<VulkanMemory> a_memory);
	~VKCubemap();
  
  virtual void Create(std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ);
  
	virtual void Bind(int textureUnit = 0) const;  
  
  virtual void SetSamplerObjectParameter(const unsigned int& parameter, const unsigned int& value);
	virtual void SetSamplerObjectParameterf(const unsigned int& parameter, float value);
  
  virtual void Release();
  
  virtual void* GetExtra(){return m_vkTextureData;}
};