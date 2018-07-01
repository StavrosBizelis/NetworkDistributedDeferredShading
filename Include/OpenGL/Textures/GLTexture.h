#pragma once

#include "Common/Textures/ATexture.h"
// Class that provides a texture for texture mapping
class GLTexture : public ATexture
{
  
  unsigned int m_textureID; // Texture id
	unsigned int m_samplerObjectID; // Sampler id 
  
  
public:
  
  GLTexture();
	~GLTexture();
  
  virtual void CreateFromData(char* data, int width, int height, int bpp, bool generateMipMaps = false);
  virtual void UpdateData(char* data, int width, int height, int bpp, bool generateMipMaps = false);
	virtual void Bind(int textureUnit = 0) const;  
  
  virtual void SetSamplerObjectParameter(const unsigned int& parameter, const unsigned int& value);
	virtual void SetSamplerObjectParameterf(const unsigned int& parameter, float value);
};