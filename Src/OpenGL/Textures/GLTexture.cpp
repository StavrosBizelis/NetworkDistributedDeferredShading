/***********************************************************************
 * AUTHOR:  <Doublecross>
 *   FILE: GLTexture.cpp
 *   DATE: Mon Jun 11 18:08:54 2018
 *  DESCR: 
 ***********************************************************************/
#include "OpenGL/Textures/GLTexture.h"
#include "gl/include/glew.h"
#include "gl/gl.h"
/*
 *  Method: GLTexture::GLTexture
 *  Params: 
 * Effects: 
 */
GLTexture::GLTexture()
{
 // Generate an OpenGL texture ID for this texture
	glGenTextures(1, &m_textureID);
	
  glGenSamplers(1, &m_samplerObjectID);
}


/*
 *  Method: GLTexture::~GLTexture
 *  Params: 
 * Effects: 
 */
GLTexture::~GLTexture()
{
  glDeleteSamplers(1, &m_samplerObjectID);
	glDeleteTextures(1, &m_textureID);
}


/*
 *  Method: GLTexture::Upload
 *  Params: 
 * Returns: void
 * Effects: 
 */
void GLTexture::CreateFromData(char* data, int width, int height, int bpp, bool generateMipMaps)
{
  UpdateData(data, width, height, bpp, generateMipMaps);

  
  
  glSamplerParameterf(m_samplerObjectID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameterf(m_samplerObjectID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  
	m_path = "";
	m_width = width;
	m_height = height;
	m_bpp = bpp;
  m_mipMapsGenerated = generateMipMaps;
	
}

void GLTexture::UpdateData(char* data, int width, int height, int bpp, bool generateMipMaps)
{
   GLenum format;
  if(bpp == 32)format = GL_BGRA;
	if(bpp == 24)format = GL_BGR;
	if(bpp == 8)format = GL_LUMINANCE;
  
  glBindTexture(GL_TEXTURE_2D, m_textureID);
  if(format == GL_RGBA || format == GL_BGRA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	// We must handle this because of internal format parameter
	else if(format == GL_RGB || format == GL_BGR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	if(generateMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
  
  m_width = width;
	m_height = height;
	m_bpp = bpp;
  m_mipMapsGenerated = generateMipMaps;
}


/*
 *  Method: GLTexture::Bind
 *  Params: int textureUnit
 * Returns: void
 * Effects: 
 */
void
GLTexture::Bind(int textureUnit) const
{
  glActiveTexture(GL_TEXTURE0+textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glBindSampler(textureUnit, m_samplerObjectID);
}
/*
 *  Method: GLTexture::SetSamplerObjectParameter
 *  Params: const unsigned int &parameter, const unsigned int &value
 * Returns: void
 * Effects: 
 */
void
GLTexture::SetSamplerObjectParameter(const unsigned int &parameter, const unsigned int &value)
{
  glSamplerParameteri(m_samplerObjectID, parameter, value);
}


/*
 *  Method: GLTexture::SetSamplerObjectParameterf
 *  Params: const unsigned int &parameter, float value
 * Returns: void
 * Effects: 
 */
void
GLTexture::SetSamplerObjectParameterf(const unsigned int &parameter, float value)
{
  glSamplerParameterf(m_samplerObjectID, parameter, value);
}


