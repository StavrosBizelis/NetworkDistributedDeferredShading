/***********************************************************************
 * AUTHOR:  <Doublecross>
 *   FILE: GLRect.cpp
 *   DATE: Mon Jun 11 16:21:13 2018
 *  DESCR: 
 ***********************************************************************/
#include "OpenGL/Shapes/GLRect.h"
#include "gl/include/glew.h"
#include "gl/gl.h"
/*
 *  Method: GLRect::GLRect
 *  Params: 
 * Effects: 
 */
GLRect::GLRect() :ARect(), m_vao(0)
{
}


/*
 *  Method: GLRect::~GLRect
 *  Params: 
 * Effects: 
 */
GLRect::~GLRect()
{
}


/*
 *  Method: GLRect::Render
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLRect::Render()
{
  	// render light mesh for stencil pass
	// bind the vao and vbo in the beginning  because we will draw these vertices twice 
	glBindVertexArray(m_vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	m_vbo.Bind();

	// do the light pass
	// render light mesh for light pass
  int l_numTriangles = 2;
	glDrawElements(GL_TRIANGLES, l_numTriangles * 3, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}


/*
 *  Method: GLRect::Release
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLRect::Release()
{
  if(m_vao)
	{ 
		glDeleteVertexArrays(1, &m_vao);
		m_vbo.Release();
		m_vao = 0;
	}
}


/*
 *  Method: GLRect::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLRect::Create()
{
  ARect::Create();
  
  glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();
  
    // upload to vbo
  for( unsigned int i = 0; i < m_vertices.size(); ++i)
  {
    m_vbo.AddVertexData( &m_vertices[i].m_pos, sizeof(glm::vec3) );
    m_vbo.AddVertexData( &m_vertices[i].m_tex, sizeof(glm::vec2) );
  }
  for( unsigned int i = 0; i < m_indices.size(); ++i)
    m_vbo.AddIndexData( &m_indices[i], sizeof(unsigned int) );
  
  
  m_vbo.UploadDataToGPU(GL_STATIC_DRAW);


	GLsizei stride = sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
}


