/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKTextureFactory.cpp
 *   DATE: Mon Jul 16 17:00:27 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/Textures/VKTextureFactory.h"

#include "Vulkan/Textures/VKTexture.h"
#include "Vulkan/Textures/VKCubemap.h"

VKTextureFactory::VKTextureFactory(std::shared_ptr<VulkanMemory> a_memory)
:m_memory(a_memory){}


/***********************************************************************
 *  Method: VKTextureFactory::GetTexture
 *  Params: 
 * Returns: std::shared_ptr<ITexture>
 * Effects: 
 ***********************************************************************/
std::shared_ptr<ITexture>
VKTextureFactory::GetTexture()
{
  std::shared_ptr<VKTexture> l_text = std::make_shared<VKTexture>(m_memory);
  l_text->Load("../Assets/EmptyTexture.jpg", true);
  return l_text;
}


/***********************************************************************
 *  Method: VKTextureFactory::GetTexture
 *  Params: std::string a_path
 * Returns: std::shared_ptr<ITexture>
 * Effects: 
 ***********************************************************************/
std::shared_ptr<ITexture>
VKTextureFactory::GetTexture(std::string a_path)
{
  std::map<std::string, std::shared_ptr<ITexture> >::iterator l_iter = m_textures.find(a_path);
  if( l_iter != m_textures.end() )
    return l_iter->second;
  
  std::shared_ptr<VKTexture> l_text = std::make_shared<VKTexture>(m_memory);
  l_text->Load(a_path, true);
  return l_text;
}


/***********************************************************************
 *  Method: VKTextureFactory::GetCubemap
 *  Params: std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ
 * Returns: std::shared_ptr<ITexture>
 * Effects: 
 ***********************************************************************/
std::shared_ptr<ITexture>
VKTextureFactory::GetCubemap(std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ)
{
  std::shared_ptr<VKCubemap> l_cubeMap = std::make_shared<VKCubemap>(m_memory);
  l_cubeMap->Create(sPositiveX, sNegativeX, sPositiveY, sNegativeY, sPositiveZ, sNegativeZ);
  return l_cubeMap;
}


