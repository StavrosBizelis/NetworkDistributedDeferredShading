/***********************************************************************
 * AUTHOR:  <Doublecross>
 *   FILE: GLSphere.cpp
 *   DATE: Mon Jun 11 16:21:26 2018
 *  DESCR: 
 ***********************************************************************/
#include "OpenGL/Shapes/GLSphere.h"
#include "gl/include/glew.h"
#include "gl/gl.h"
/*
 *  Method: GLSphere::GLSphere
 *  Params: 
 * Effects: 
 */
GLSphere::GLSphere() : ASphere()
{
}


/*
 *  Method: GLSphere::~GLSphere
 *  Params: 
 * Effects: 
 */
GLSphere::~GLSphere()
{ 
  Release();
}


/*
 *  Method: GLSphere::Render
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLSphere::Render()
{
  glBindVertexArray(m_vao);
  m_vbo.Bind();
	glDrawElements(GL_TRIANGLES, m_numTriangles*3, GL_UNSIGNED_INT, 0);
}


/*
 *  Method: GLSphere::Release
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLSphere::Release()
{
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}


/*
 *  Method: GLSphere::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLSphere::Create()
{
  ASphere::Create();


	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();

  // upload to vbo
  for( unsigned int i = 0; i < m_vertices.size(); ++i)
  {
    m_vbo.AddVertexData( &m_vertices[i].m_pos, sizeof(glm::vec3) );
    m_vbo.AddVertexData( &m_vertices[i].m_tex, sizeof(glm::vec2) );
    m_vbo.AddVertexData( &m_vertices[i].m_normal, sizeof(glm::vec3) );
  }
  for( unsigned int i = 0; i < m_indices.size(); ++i)
    m_vbo.AddIndexData( &m_indices[i], sizeof(unsigned int) );
  

  
  m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
  
  
  
}


