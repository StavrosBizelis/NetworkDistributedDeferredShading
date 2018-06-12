/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ASkybox.cpp
 *   DATE: Mon Jun 11 14:14:49 2018
 *  DESCR: 
 ***********************************************************************/
#include "Common/Shapes/ASkybox.h"

/*
 *  Method: ASkybox::ASkybox
 *  Params: 
 * Effects: 
 */
ASkybox::ASkybox()
{
}


/*
 *  Method: ASkybox::~ASkybox
 *  Params: 
 * Effects: 
 */
ASkybox::~ASkybox()
{
}


/*
 *  Method: ASkybox::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void ASkybox::Create()
{
  float l_edgeHalfLength = 500; 
  
  glm::vec3 l_vertices[24] = 
	{
		// Front face
		glm::vec3(l_edgeHalfLength, l_edgeHalfLength, l_edgeHalfLength), glm::vec3(l_edgeHalfLength, -l_edgeHalfLength, l_edgeHalfLength), glm::vec3(-l_edgeHalfLength, l_edgeHalfLength, l_edgeHalfLength), glm::vec3(-l_edgeHalfLength, -l_edgeHalfLength, l_edgeHalfLength),
		// Back face
		glm::vec3(-l_edgeHalfLength, l_edgeHalfLength, -l_edgeHalfLength), glm::vec3(-l_edgeHalfLength, -l_edgeHalfLength, -l_edgeHalfLength), glm::vec3(l_edgeHalfLength, l_edgeHalfLength, -l_edgeHalfLength), glm::vec3(l_edgeHalfLength, -l_edgeHalfLength, -l_edgeHalfLength),
		// Left face
		glm::vec3(-l_edgeHalfLength, l_edgeHalfLength, l_edgeHalfLength), glm::vec3(-l_edgeHalfLength, -l_edgeHalfLength, l_edgeHalfLength), glm::vec3(-l_edgeHalfLength, l_edgeHalfLength, -l_edgeHalfLength), glm::vec3(-l_edgeHalfLength, -l_edgeHalfLength, -l_edgeHalfLength),
		// Right face
		glm::vec3(l_edgeHalfLength, l_edgeHalfLength, -l_edgeHalfLength), glm::vec3(l_edgeHalfLength, -l_edgeHalfLength, -l_edgeHalfLength), glm::vec3(l_edgeHalfLength, l_edgeHalfLength, l_edgeHalfLength), glm::vec3(l_edgeHalfLength, -l_edgeHalfLength, l_edgeHalfLength),
		// Top face
		glm::vec3(-l_edgeHalfLength, l_edgeHalfLength, -l_edgeHalfLength), glm::vec3(l_edgeHalfLength, l_edgeHalfLength, -l_edgeHalfLength), glm::vec3(-l_edgeHalfLength, l_edgeHalfLength, l_edgeHalfLength), glm::vec3(l_edgeHalfLength, l_edgeHalfLength, l_edgeHalfLength),
		// Bottom face
		glm::vec3(l_edgeHalfLength, -l_edgeHalfLength, -l_edgeHalfLength), glm::vec3(-l_edgeHalfLength, -l_edgeHalfLength, -l_edgeHalfLength), glm::vec3(l_edgeHalfLength, -l_edgeHalfLength, l_edgeHalfLength), glm::vec3(-l_edgeHalfLength, -l_edgeHalfLength, l_edgeHalfLength),
	};
  
  
	glm::vec2 l_textCoord[4] =
	{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
	};
  

	glm::vec3 l_normals[6] = 
	{
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

  for (int i = 0; i < 24; i++)
    m_vertices.push_back( Vertex( l_vertices[i], l_textCoord[i%4], l_normals[i/4], glm::vec3(0) ) );
  
}


