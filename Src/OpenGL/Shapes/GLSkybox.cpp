/***********************************************************************
 * AUTHOR:  <Doublecross>
 *   FILE: GLSkybox.cpp
 *   DATE: Mon Jun 11 16:21:19 2018
 *  DESCR: 
 ***********************************************************************/
#include "OpenGL/Shapes/GLSkybox.h"

/*
 *  Method: GLSkybox::GLSkybox
 *  Params: 
 * Effects: 
 */
GLSkybox::GLSkybox() : m_vao ( 0 )
{
}


/*
 *  Method: GLSkybox::~GLSkybox
 *  Params: 
 * Effects: 
 */
GLSkybox::~GLSkybox()
{
  Release();
}


/*
 *  Method: GLSkybox::Render
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLSkybox::Render()
{
  //glDepthMask(0);
	glBindVertexArray(m_vao);
	//m_cubemapTexture.Bind(textureUnit);
	for (int i = 0; i < 6; i++) {
	//	//m_textures[i].Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4);
	}
	//glDepthMask(1);
}


/*
 *  Method: GLSkybox::Release
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLSkybox::Release()
{
  if( m_vao != 0 )
  {
    //for (int i = 0; i < 6; i++)
      //m_textures[i].Release();
    // m_cubemapTexture.Release();
    glDeleteVertexArrays(1, &m_vao);
    m_vbo.Release();
    m_vao = 0;
  }
}


/*
 *  Method: GLSkybox::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLSkybox::Create()
{
  ASkybox::Create();
  
  glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();
  
  
  // upload to vbo
  for( unsigned int i = 0; i < m_vertices.size(); ++i)
  {
    m_vbo.AddData( &m_vertices[i].m_pos, sizeof(glm::vec3) );
    m_vbo.AddData( &m_vertices[i].m_tex, sizeof(glm::vec2) );
    m_vbo.AddData( &m_vertices[i].m_normal, sizeof(glm::vec3) );
  }

  
  
  m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei istride = 2*sizeof(glm::vec3)+sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));
  
}


