#include "OpenGL/Textures/GLTextureFactory.h"
#include "OpenGL/Textures/GLTexture.h"
#include "OpenGL/Textures/GLCubemap.h"


std::shared_ptr<ITexture> GLTextureFactory::GetTexture()
{
  std::shared_ptr<GLTexture> l_text = std::make_shared<GLTexture>();
  l_text->Load("../Assets/EmptyTexture.png", true);
  return l_text;
}

std::shared_ptr<ITexture> GLTextureFactory::GetTexture(std::string a_path)
{
  std::map<std::string, std::shared_ptr<ITexture> >::iterator l_iter = m_textures.find(a_path);
  if( l_iter != m_textures.end() )
    return l_iter->second;
  
  std::shared_ptr<GLTexture> l_text = std::make_shared<GLTexture>();
  l_text->Load(a_path, true);
  m_textures[a_path] = l_text;
  return l_text;
}
std::shared_ptr<ITexture> GLTextureFactory::GetCubemap(std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ)
{
  std::shared_ptr<GLCubemap> l_cubeMap = std::make_shared<GLCubemap>();
  l_cubeMap->Create(sPositiveX, sNegativeX, sPositiveY, sNegativeY, sPositiveZ, sNegativeZ);
  return l_cubeMap;
}