/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKTextureFactory.cpp
 *   DATE: Mon Jul 16 17:00:27 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/Textures/VKTextureFactory.h"

/***********************************************************************
 *  Method: VKTextureFactory::GetTexture
 *  Params: 
 * Returns: std::shared_ptr<ITexture>
 * Effects: 
 ***********************************************************************/
std::shared_ptr<ITexture>
VKTextureFactory::GetTexture()
{
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
}


