#pragma once

#include "Common/Textures/ACubemap.h"
#include <string>
// Class that provides a texture for texture mapping
class GLCubemap : public ACubemap
{
  
  unsigned int m_uiTexture;
	unsigned int m_uiSampler; // Sampler name
  
public:
  
  GLCubemap();
	~GLCubemap();
  
  virtual void Create(std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ);
  
	virtual void Bind(int textureUnit = 0) const;  
  
  virtual void SetSamplerObjectParameter(const unsigned int& parameter, const unsigned int& value);
	virtual void SetSamplerObjectParameterf(const unsigned int& parameter, float value);
  
  virtual void Release();
};