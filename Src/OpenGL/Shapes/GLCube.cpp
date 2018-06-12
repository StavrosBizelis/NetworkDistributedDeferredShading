/***********************************************************************
 * AUTHOR:  <Doublecross>
 *   FILE: GLCube.cpp
 *   DATE: Mon Jun 11 16:20:49 2018
 *  DESCR: 
 ***********************************************************************/
#include "OpenGL/Shapes/GLCube.h"
#include "gl/include/glew.h"
#include "gl/gl.h"
/*
 *  Method: GLCube::GLCube
 *  Params: 
 * Effects: 
 */
GLCube::GLCube()
{
}


/*
 *  Method: GLCube::~GLCube
 *  Params: 
 * Effects: 
 */
GLCube::~GLCube()
{
  Release();
}


/*
 *  Method: GLCube::Render
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLCube::Render()
{
  glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
}


/*
 *  Method: GLCube::Release
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLCube::Release()
{
  if (m_vao)
	{
		glDeleteVertexArrays(1, &m_vao);
		m_vbo.Release();
	}
}


/*
 *  Method: GLCube::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
GLCube::Create()
{
  ACube::Create();
  
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


