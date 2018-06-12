#pragma once

#include "Common/Textures/ITexture.h"
#include <string>
// Class that provides a texture for texture mapping
class ACubemap : public ITexture
{
public:
  
  ACubemap();
	~ACubemap();
  
  virtual void Create(std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ) = 0;
	
	bool LoadTexture(std::string filename, char **bmpBytes, int &iWidth, int &iHeight);
  
  virtual void Release() = 0;
protected:
	bool m_mipMapsGenerated;
  
};