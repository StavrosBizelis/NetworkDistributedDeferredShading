/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ACone.cpp
 *   DATE: Mon Jun 11 14:14:03 2018
 *  DESCR: 
 ***********************************************************************/
#include "Common/Shapes/ACone.h"

/*
 *  Method: ACone::ACone
 *  Params: 
 * Effects: 
 */
ACone::ACone()
{
}


/*
 *  Method: ACone::~ACone
 *  Params: 
 * Effects: 
 */
ACone::~ACone()
{
}


/*
 *  Method: ACone::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void ACone::Create()
{
  m_vertices.push_back( Vertex( glm::vec3(0,0,0),glm::vec2(0), glm::vec3(0), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(1,-1,0),glm::vec2(0), glm::vec3(0), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(0.707107,-1,0.707107),glm::vec2(0), glm::vec3(0), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(0,-1,1),glm::vec2(0), glm::vec3(0), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(-0.707107, -1, 0.707107),glm::vec2(0), glm::vec3(0), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(-1,-1,0),glm::vec2(0), glm::vec3(0), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(-0.707107, -1, -0.707107),glm::vec2(0), glm::vec3(0), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(0, -1, -1),glm::vec2(0), glm::vec3(0), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(0.707107, -1, -0.707107),glm::vec2(0), glm::vec3(0), glm::vec3(0))  );
  
  
  m_indices = 
    { 0,2,1,  0,3,2,  0,4,3,  0,5,4,  0,6,5,  0,7,6,  0,8,7,  0,1,8,         // fan
      1,2,8,   2,7,8,   2,3,7,    3,6,7,   3,4,6,   4,5,6   // base
    };
    
  m_numTriangles = 14;
}


