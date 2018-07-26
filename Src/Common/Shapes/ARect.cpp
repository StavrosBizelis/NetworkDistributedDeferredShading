/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ARect.cpp
 *   DATE: Mon Jun 11 14:14:44 2018
 *  DESCR: 
 ***********************************************************************/
#include "Common/Shapes/ARect.h"

/*
 *  Method: ARect::ARect
 *  Params: 
 * Effects: 
 */
ARect::ARect()
{
}


/*
 *  Method: ARect::~ARect
 *  Params: 
 * Effects: 
 */
ARect::~ARect()
{
}


/*
 *  Method: ARect::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void ARect::Create()
{
   // front
  m_vertices.push_back( Vertex( glm::vec3(-1, -1, 0),glm::vec2(0, 0), glm::vec3(.0f,.0f,1.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(1, -1, 0),glm::vec2(1, 0), glm::vec3(.0f,.0f,1.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(-1, 1, 0),glm::vec2(0, 1), glm::vec3(.0f,.0f,1.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(1, 1, 0),glm::vec2(1,1), glm::vec3(.0f,.0f,1.0f), glm::vec3(0))  );
  

  m_indices = {0,1,2,1,3,2};

  
}


