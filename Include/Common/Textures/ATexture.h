#pragma once

#include "Common/Textures/ITexture.h"
#include <string>
// Class that provides a texture for texture mapping
class ATexture : public ITexture
{
public:
  
  ATexture();
	~ATexture();
  
	bool Load(std::string path, bool generateMipMaps = true);

	int GetWidth();
	int GetHeight();
	int GetBPP();
  
  
  virtual void CreateFromData(char* data, int width, int height, int bpp, bool generateMipMaps = false) = 0;
  virtual void UpdateData(char* data, int width, int height, int bpp, bool generateMipMaps = false) = 0;
  
  
protected:
	int m_width, m_height, m_bpp; // Texture width, height, and bytes per pixel

	bool m_mipMapsGenerated;

	std::string m_path;
};