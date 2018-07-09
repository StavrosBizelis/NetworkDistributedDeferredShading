#pragma once

#include "Common/Textures/ITexture.h"
#include <memory>
#include <string>

class ITextureFactory
{
public:
  virtual std::shared_ptr<ITexture> GetTexture() = 0;
  virtual std::shared_ptr<ITexture> GetTexture(std::string path) = 0;
  virtual std::shared_ptr<ITexture> GetCubemap(std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ) = 0;
};