/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKTexture.cpp
 *   DATE: Mon Jul 16 17:00:23 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/Textures/VKTexture.h"

/***********************************************************************
 *  Method: VKTexture::VKTexture
 *  Params: 
 * Effects: 
 ***********************************************************************/
VKTexture::VKTexture(std::shared_ptr<VulkanMemory> a_memory)
  : m_memory(a_memory){}


/***********************************************************************
 *  Method: VKTexture::~VKTexture
 *  Params: 
 * Effects: 
 ***********************************************************************/
VKTexture::~VKTexture()
{
  if(m_image)
    m_image->Free();
}


/***********************************************************************
 *  Method: VKTexture::CreateFromData
 *  Params: char *data, int width, int height, int bpp, bool generateMipMaps
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKTexture::CreateFromData(char *data, int width, int height, int bpp, bool generateMipMaps)
{
  UpdateData(data, width, height, bpp, generateMipMaps);
  
  m_sampler= std::make_shared<VulkanSampler>( m_memory->GetLogicalDevice() );
  
	m_path = "";
	m_width = width;
	m_height = height;
	m_bpp = bpp;
  m_mipMapsGenerated = generateMipMaps;
}


/***********************************************************************
 *  Method: VKTexture::UpdateData
 *  Params: char *data, int width, int height, int bpp, bool generateMipMaps
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKTexture::UpdateData(char *data, int width, int height, int bpp, bool generateMipMaps)
{
  if( !m_memory )
    return;
  VkFormat  format;
  if(bpp == 32)format = VK_FORMAT_B8G8R8A8_UINT;
	if(bpp == 24)format = VK_FORMAT_B8G8R8_UINT;
	if(bpp == 8)format = VK_FORMAT_R8_UINT;
  
  // if for some reason its impossible to update the texture - create a new image
  if( !m_memory->UpdateTextureData(m_image, data, width, height, format) )
  {
    if( m_image )
      m_image->Free();
    m_image = m_memory->CreateMaterialTexture(data, width, height, format);
  }
  

	// if(generateMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
  
  m_width = width;
	m_height = height;
	m_bpp = bpp;
  m_mipMapsGenerated = generateMipMaps;
}


/***********************************************************************
 *  Method: VKTexture::Bind
 *  Params: int textureUnit
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKTexture::Bind(int textureUnit) const
{
}


/***********************************************************************
 *  Method: VKTexture::SetSamplerObjectParameter
 *  Params: const unsigned int &parameter, const unsigned int &value
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKTexture::SetSamplerObjectParameter(const unsigned int &parameter, const unsigned int &value)
{
  if(m_sampler)
    m_sampler->SetSamplerObjectParameter((VulkanSamplerOption)parameter, value);
}


/***********************************************************************
 *  Method: VKTexture::SetSamplerObjectParameterf
 *  Params: const unsigned int &parameter, float value
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
VKTexture::SetSamplerObjectParameterf(const unsigned int &parameter, float value)
{
  if(m_sampler)
    m_sampler->SetSamplerObjectParameter((VulkanSamplerOption)parameter, value);
}


