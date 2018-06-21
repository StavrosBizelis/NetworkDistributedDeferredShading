/***********************************************************************
 * AUTHOR:  <Doublecross>
 *   FILE: GLCubemap.cpp
 *   DATE: Mon Jun 11 18:08:49 2018
 *  DESCR: 
 ***********************************************************************/
#include "OpenGL/Textures/GLCubemap.h"
#include "gl/include/glew.h"
#include "gl/gl.h"
/*
 *  Method: GLCubemap::GLCubemap
 *  Params: 
 * Effects: 
 */
GLCubemap::GLCubemap()
{
}


/*
 *  Method: GLCubemap::~GLCubemap
 *  Params: 
 * Effects: 
 */
GLCubemap::~GLCubemap()
{
  glDeleteSamplers(1, &m_uiSampler);
	glDeleteTextures(1, &m_uiTexture);
}



void GLCubemap::Create(std::string sPositiveX, std::string sNegativeX, std::string sPositiveY, std::string sNegativeY, std::string sPositiveZ, std::string sNegativeZ)
{
  int iWidth, iHeight;

	// Generate an OpenGL texture ID for this texture
	glGenTextures(1, &m_uiTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_uiTexture);

	// Load the six sides
	char *pbImagePosX, *pbImageNegX, *pbImagePosY, *pbImageNegY, *pbImagePosZ, *pbImageNegZ;

	LoadTexture(sPositiveX, &pbImagePosX, iWidth, iHeight);
	LoadTexture(sNegativeX, &pbImageNegX, iWidth, iHeight);
	LoadTexture(sPositiveY, &pbImagePosY, iWidth, iHeight);
	LoadTexture(sNegativeY, &pbImageNegY, iWidth, iHeight);
	LoadTexture(sPositiveZ, &pbImagePosZ, iWidth, iHeight);
	LoadTexture(sNegativeZ, &pbImageNegZ, iWidth, iHeight);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pbImagePosX);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pbImageNegX);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pbImagePosY);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pbImageNegY);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pbImagePosZ);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pbImageNegZ);

	delete[] pbImagePosX;
	delete[] pbImageNegX;
	delete[] pbImagePosY;
	delete[] pbImageNegY;
	delete[] pbImagePosZ;
	delete[] pbImageNegZ;

	glGenSamplers(1, &m_uiSampler);
	glSamplerParameteri(m_uiSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glSamplerParameteri(m_uiSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_uiSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_uiSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
} 

/*
 *  Method: GLCubemap::Bind
 *  Params: int textureUnit
 * Returns: void
 * Effects: 
 */
void
GLCubemap::Bind(int textureUnit) const
{
  glActiveTexture(GL_TEXTURE0+textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_uiTexture);
	glBindSampler(textureUnit, m_uiSampler);
}


/*
 *  Method: GLCubemap::SetSamplerObjectParameter
 *  Params: const unsigned int &parameter, const unsigned int &value
 * Returns: void
 * Effects: 
 */
void
GLCubemap::SetSamplerObjectParameter(const unsigned int &parameter, const unsigned int &value)
{
  glSamplerParameterf(m_uiSampler, parameter, value);
}


/*
 *  Method: GLCubemap::SetSamplerObjectParameterf
 *  Params: const unsigned int &parameter, float value
 * Returns: void
 * Effects: 
 */
void
GLCubemap::SetSamplerObjectParameterf(const unsigned int &parameter, float value)
{
  glSamplerParameterf(m_uiSampler, parameter, value);
}


void GLCubemap::Release()
{
  glDeleteSamplers(1, &m_uiSampler);
	glDeleteTextures(1, &m_uiTexture);
} 

