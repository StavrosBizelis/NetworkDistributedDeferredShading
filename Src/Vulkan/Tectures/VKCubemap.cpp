/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKCubemap.cpp
 *   DATE: Mon Jul 16 17:00:17 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/Textures/VKCubemap.h"

/***********************************************************************
 *  Method: VKCubemap::VKCubemap
 *  Params: 
 * Effects: 
 ***********************************************************************/
VKCubemap::VKCubemap(std::shared_ptr<VulkanMemory> a_memory)
  : m_memory(a_memory){}


/***********************************************************************
 *  Method: VKCubemap::~VKCubemap
 *  Params: 
 * Effects: 
 ***********************************************************************/
VKCubemap::~VKCubemap()
{  
  Release();
}


/***********************************************************************
 *  Method: VKCubemap::Create
 *  Params: std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKCubemap::Create(std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ)
{
  int iWidth, iHeight;
  char *pbImagePosX, *pbImageNegX, *pbImagePosY, *pbImageNegY, *pbImagePosZ, *pbImageNegZ;
  LoadTexture(sPositiveX, &pbImagePosX, iWidth, iHeight);
  LoadTexture(sNegativeX, &pbImageNegX, iWidth, iHeight);
  LoadTexture(sPositiveY, &pbImagePosY, iWidth, iHeight);
  LoadTexture(sNegativeY, &pbImageNegY, iWidth, iHeight);
  LoadTexture(sPositiveZ, &pbImagePosZ, iWidth, iHeight);
  LoadTexture(sNegativeZ, &pbImageNegZ, iWidth, iHeight);
  
  
  //  create a new cubmap - we are not updating with new data - just create it
  m_image = m_memory->CreateCubemap(pbImagePosX, pbImageNegX, pbImagePosY, pbImageNegY, pbImagePosZ, pbImageNegZ, iWidth, iHeight, VK_FORMAT_B8G8R8A8_UINT);
  
  m_sampler= std::make_shared<VulkanSampler>();

	// if(generateMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
  
  m_width = width;
	m_height = height;
	m_bpp = bpp;
  m_mipMapsGenerated = generateMipMaps;
  
}


/***********************************************************************
 *  Method: VKCubemap::Bind
 *  Params: int textureUnit
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKCubemap::Bind(int textureUnit) const
{
  VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  
  
}


/***********************************************************************
 *  Method: VKCubemap::SetSamplerObjectParameter
 *  Params: const unsigned int &parameter, const unsigned int &value
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKCubemap::SetSamplerObjectParameter(const unsigned int &parameter, const unsigned int &value)
{
  if(m_sampler)
    m_sampler->SetSamplerObjectParameter(parameter, value);
}


/***********************************************************************
 *  Method: VKCubemap::SetSamplerObjectParameterf
 *  Params: const unsigned int &parameter, float value
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKCubemap::SetSamplerObjectParameterf(const unsigned int &parameter, float value)
{
  if(m_sampler)
    m_sampler->SetSamplerObjectParameter(parameter, value);
}


/***********************************************************************
 *  Method: VKCubemap::Release
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKCubemap::Release()
{
  if(m_image)
    m_image->Free();
}


