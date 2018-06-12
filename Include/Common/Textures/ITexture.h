#pragma once

/// Basic interface for Textures
class ITexture
{
public:

	virtual void Bind(int textureUnit = 0) const = 0;  
  
  virtual void SetSamplerObjectParameter(const unsigned int& parameter, const unsigned int& value) = 0;
	virtual void SetSamplerObjectParameterf(const unsigned int& parameter, float value) = 0;

};