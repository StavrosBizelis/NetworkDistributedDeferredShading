#pragma once

#include "Common/Textures/ACubemap.h"
#include <string>
// Class that provides a texture for texture mapping
class VKCubemap : public ACubemap
{
  
  std::shared_ptr<VulkanMemory> m_memory;
  std::shared_ptr<VulkanImageMemoryChunk> m_image;
  std::shared_ptr<VulkanSampler> m_sampler;
public:
  
  VKCubemap(std::shared_ptr<VulkanMemory> a_memory);
	~VKCubemap();
  
  virtual void Create(std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ);
  
	virtual void Bind(int textureUnit = 0) const;  
  
  virtual void SetSamplerObjectParameter(const unsigned int& parameter, const unsigned int& value);
	virtual void SetSamplerObjectParameterf(const unsigned int& parameter, float value);
  
  virtual void Release();
};