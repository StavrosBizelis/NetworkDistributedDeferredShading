/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKRect.cpp
 *   DATE: Fri Jul 13 19:30:00 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/Shapes/VKRect.h"
#include <iostream>
/*
 *  Method: VKRect::VKRect
 *  Params: 
 * Effects: 
 */
VKRect::VKRect(const std::shared_ptr<VulkanMemory>& a_memory) :ARect(), m_memory(a_memory), m_vulkanShape(nullptr)
{
}


/*
 *  Method: VKRect::~VKRect
 *  Params: 
 * Effects: 
 */
VKRect::~VKRect()
{
  Release();
}


/*
 *  Method: VKRect::Render
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKRect::Render()
{
  

}


/*
 *  Method: VKRect::Release
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKRect::Release()
{
  if( m_vulkanShape )
    delete m_vulkanShape;
}


/*
 *  Method: VKRect::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKRect::Create()
{
  ARect::Create();
  
  m_vertices.clear();
  m_vertices.push_back( Vertex( glm::vec3(0, 0, 1),glm::vec2(0, 0), glm::vec3(.0f,.0f,1.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(0, 1, 0),glm::vec2(1, 0), glm::vec3(.0f,.0f,1.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(1, 0, 0),glm::vec2(0, 1), glm::vec3(.0f,.0f,1.0f), glm::vec3(0))  );
  m_vertices.push_back( Vertex( glm::vec3(1, 1, 0),glm::vec2(1,1), glm::vec3(.0f,.0f,1.0f), glm::vec3(0))  );
  
  
  // upload data to the gpu
  std::shared_ptr<VulkanMemoryChunk> l_vkVertices = m_memory->CreateVertexBuffer( (char*)m_vertices.data(), m_vertices.size() * sizeof(Vertex) );
  std::shared_ptr<VulkanMemoryChunk> l_vkIndices = m_memory->CreateIndexBuffer( (char*)m_indices.data(), m_indices.size() * sizeof(unsigned int) );
  
  m_vulkanShape = new VKShape(m_memory, {l_vkVertices}, {l_vkIndices}, {m_indices.size()});
	// VKsizei stride = sizeof(glm::vec3) + sizeof(glm::vec2);
	// // Vertex positions
	// VKEnableVertexAttribArray(0);
	// VKEnableVertexAttribArray(1);

	// VKVertexAttribPointer(0, 3, VK_FLOAT, VK_FALSE, stride, 0);
	// VKVertexAttribPointer(1, 2, VK_FLOAT, VK_FALSE, stride, (void*)sizeof(glm::vec3));
}


