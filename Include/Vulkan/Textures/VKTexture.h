#pragma once

#include "Common/Textures/ATexture.h"
#include "Vulkan/Textures/VKATexture.h"
#include "Vulkan/Core/VulkanMemory.h"
#include <memory>
// Class that provides a texture for texture mapping
class VKTexture : public ATexture
{
  
  std::shared_ptr<VulkanMemory> m_memory;
  VKATexture* m_vkTextureData;
public:
  
  VKTexture(std::shared_ptr<VulkanMemory> a_memory);
	~VKTexture();
  
  virtual void CreateFromData(char* data, int width, int height, int bpp, bool generateMipMaps = false);
  virtual void UpdateData(char* data, int width, int height, int bpp, bool generateMipMaps = false);
	virtual void Bind(int textureUnit = 0) const;  
  
  virtual void SetSamplerObjectParameter(const unsigned int& parameter, const unsigned int& value);
	virtual void SetSamplerObjectParameterf(const unsigned int& parameter, float value);
  
  virtual void* GetExtra(){return m_vkTextureData;}
};