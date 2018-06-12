/***********************************************************************
 * AUTHOR:  <Doublecross>
 *   FILE: GLCone.cpp
 *   DATE: Mon Jun 11 16:20:46 2018
 *  DESCR: 
 ***********************************************************************/
#include "OpenGL/Shapes/GLCone.h"
#include "gl/include/glew.h"
#include "gl/gl.h"
/*
 *  Method: GLCone::GLCone
 *  Params: 
 * Effects: 
 */
GLCone::GLCone() : ACone(), m_vao(0)
{
}


/*
 *  Method: GLCone::~GLCone
 *  Params: 
 * Effects: 
 */
GLCone::~GLCone()
{
  Release();
}


/*
 *  Method: GLCone::Render
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLCone::Render()
{
  glBindVertexArray(m_vao);
	glEnableVertexAttribArray(0);
	m_vbo.Bind();

	glDrawElements(GL_TRIANGLES, m_numTriangles * 3, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
}


/*
 *  Method: GLCone::Release
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLCone::Release()
{
  if (m_vao)
	{
		glDeleteVertexArrays(1, &m_vao);
		m_vbo.Release();
	}
}


/*
 *  Method: GLCone::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLCone::Create()
{
  ACone::Create();
  
  glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();
  
   // upload to vbo
  for( unsigned int i = 0; i < m_vertices.size(); ++i)
    m_vbo.AddVertexData( &m_vertices[i].m_pos, sizeof(glm::vec3) );
  for( unsigned int i = 0; i < m_indices.size(); ++i)
    m_vbo.AddIndexData( &m_indices[i], sizeof(unsigned int) );
  
  m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = sizeof(glm::vec3);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
  
}


