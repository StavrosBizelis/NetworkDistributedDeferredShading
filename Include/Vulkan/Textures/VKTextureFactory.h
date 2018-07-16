#pragma once

#include "Common/Textures/ITextureFactory.h"
#include <map>
#include <string>

class VKTextureFactory : public ITextureFactory
{
private:
  std::shared_ptr<VulkanMemory> m_memory;
  std::map<std::string, std::shared_ptr<ITexture> > m_textures;
public:
  VKTextureFactory(std::shared_ptr<VulkanMemory> a_memory);
  virtual std::shared_ptr<ITexture> GetTexture();
  virtual std::shared_ptr<ITexture> GetTexture(std::string a_path);
  virtual std::shared_ptr<ITexture> GetCubemap(std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ);
};