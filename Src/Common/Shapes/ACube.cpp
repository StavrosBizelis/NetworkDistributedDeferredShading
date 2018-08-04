/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ACube.cpp
 *   DATE: Mon Jun 11 14:14:13 2018
 *  DESCR: 
 ***********************************************************************/
#include "Common/Shapes/ACube.h"

/*
 *  Method: ACube::ACube
 *  Params: 
 * Effects: 
 */
ACube::ACube()
{
}


/*
 *  Method: ACube::~ACube
 *  Params: 
 * Effects: 
 */
ACube::~ACube()
{
}


/*
 *  Method: ACube::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void ACube::Create()
{
  float l_edgeHalfLength = 0.5;
  
  // front
  m_vertices.push_back( Vertex( glm::vec3(-l_edgeHalfLength,l_edgeHalfLength,l_edgeHalfLength),glm::vec2(0,1), glm::vec3(.0f,.0f,1.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(-l_edgeHalfLength,-l_edgeHalfLength,l_edgeHalfLength),glm::vec2(0,0), glm::vec3(.0f,.0f,1.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(l_edgeHalfLength,l_edgeHalfLength,l_edgeHalfLength),glm::vec2(1,1), glm::vec3(.0f,.0f,1.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(l_edgeHalfLength,-l_edgeHalfLength,l_edgeHalfLength),glm::vec2(1,0), glm::vec3(.0f,.0f,1.0f), glm::vec3(0))  );

  // right
  m_vertices.push_back( Vertex( glm::vec3(l_edgeHalfLength,l_edgeHalfLength,l_edgeHalfLength),glm::vec2(0,1), glm::vec3(1.0f,.0f,.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(l_edgeHalfLength,-l_edgeHalfLength,l_edgeHalfLength),glm::vec2(0,0), glm::vec3(1.0f,.0f,.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(l_edgeHalfLength,l_edgeHalfLength,-l_edgeHalfLength),glm::vec2(1,1), glm::vec3(1.0f,.0f,.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(l_edgeHalfLength,-l_edgeHalfLength,-l_edgeHalfLength),glm::vec2(1,0), glm::vec3(1.0f,.0f,.0f), glm::vec3(0))  );
  
  // top face
  m_vertices.push_back( Vertex( glm::vec3(l_edgeHalfLength,l_edgeHalfLength,l_edgeHalfLength),glm::vec2(0,1), glm::vec3(.0f,1.0f,.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(l_edgeHalfLength,l_edgeHalfLength,-l_edgeHalfLength),glm::vec2(0,0), glm::vec3(.0f,1.0f,.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(-l_edgeHalfLength,l_edgeHalfLength,l_edgeHalfLength),glm::vec2(1,1), glm::vec3(.0f,1.0f,.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(-l_edgeHalfLength,l_edgeHalfLength,-l_edgeHalfLength),glm::vec2(1,0), glm::vec3(.0f,1.0f,.0f), glm::vec3(0))  );
  

  // back
  m_vertices.push_back( Vertex( glm::vec3(-l_edgeHalfLength,-l_edgeHalfLength,-l_edgeHalfLength),glm::vec2(0,1), glm::vec3(.0f,.0f,-1.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(-l_edgeHalfLength,l_edgeHalfLength,-l_edgeHalfLength),glm::vec2(0,0), glm::vec3(.0f,.0f,-1.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(l_edgeHalfLength,-l_edgeHalfLength,-l_edgeHalfLength),glm::vec2(1,1), glm::vec3(.0f,.0f,-1.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(l_edgeHalfLength,l_edgeHalfLength,-l_edgeHalfLength),glm::vec2(1,0), glm::vec3(.0f,.0f,-1.0f), glm::vec3(0))  );

  
  // left 
  m_vertices.push_back( Vertex( glm::vec3(-l_edgeHalfLength,l_edgeHalfLength,-l_edgeHalfLength),glm::vec2(0,1), glm::vec3(-1.0f,.0f,.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(-l_edgeHalfLength,-l_edgeHalfLength,-l_edgeHalfLength),glm::vec2(0,0), glm::vec3(-1.0f,.0f,.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(-l_edgeHalfLength,l_edgeHalfLength,l_edgeHalfLength),glm::vec2(1,1), glm::vec3(-1.0f,.0f,.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(-l_edgeHalfLength,-l_edgeHalfLength,l_edgeHalfLength),glm::vec2(1,0), glm::vec3(-1.0f,.0f,.0f), glm::vec3(0))  );


  // bottom face
  m_vertices.push_back( Vertex( glm::vec3(-l_edgeHalfLength,-l_edgeHalfLength,l_edgeHalfLength),glm::vec2(0,1), glm::vec3(.0f,-1.0f,.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(-l_edgeHalfLength,-l_edgeHalfLength,-l_edgeHalfLength),glm::vec2(0,0), glm::vec3(.0f,-1.0f,.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(l_edgeHalfLength,-l_edgeHalfLength,l_edgeHalfLength),glm::vec2(1,1), glm::vec3(.0f,-1.0f,.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(l_edgeHalfLength,-l_edgeHalfLength,-l_edgeHalfLength),glm::vec2(1,0), glm::vec3(.0f,-1.0f,.0f), glm::vec3(0))  );

  
}


