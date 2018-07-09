#pragma once

#include "Common/Textures/ITextureFactory.h"
#include <map>
#include <string>

class GLTextureFactory : public ITextureFactory
{
private:
  std::map<std::string, std::shared_ptr<ITexture> > m_textures;
public:
  virtual std::shared_ptr<ITexture> GetTexture();
  virtual std::shared_ptr<ITexture> GetTexture(std::string a_path);
  virtual std::shared_ptr<ITexture> GetCubemap(std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ);
};