/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKSphere.cpp
 *   DATE: Fri Jul 13 19:30:00 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/Shapes/VKSphere.h"

/*
 *  Method: VKSphere::VKSphere
 *  Params: 
 * Effects: 
 */
VKSphere::VKSphere(const std::shared_ptr<VulkanMemory>& a_memory) : ASphere(), m_memory(a_memory), m_vulkanShape(nullptr)
{
}


/*
 *  Method: VKSphere::~VKSphere
 *  Params: 
 * Effects: 
 */
VKSphere::~VKSphere()
{ 
  Release();
}


/*
 *  Method: VKSphere::Render
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKSphere::Render()
{
  // VKBindVertexArray(m_vao);
  // m_vbo.Bind();
	// VKDrawElements(VK_TRIANVKES, m_numTrianVKes*3, VK_UNSIGNED_INT, 0);
}


/*
 *  Method: VKSphere::Release
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKSphere::Release()
{
  if( m_vulkanShape )
    delete m_vulkanShape;
}


/*
 *  Method: VKSphere::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKSphere::Create()
{
  ASphere::Create();

  // upload data to the gpu
  std::shared_ptr<VulkanMemoryChunk> l_vkVertices = m_memory->CreateVertexBuffer( (char*)m_vertices.data(), m_vertices.size() * sizeof(Vertex) );
  std::shared_ptr<VulkanMemoryChunk> l_vkIndices = m_memory->CreateIndexBuffer( (char*)m_indices.data(), m_indices.size() * sizeof(unsigned int) );
  
  m_vulkanShape = new VKShape(m_memory, {l_vkVertices}, {l_vkIndices}, {m_indices.size()});

	// VKsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// // Vertex positions
	// VKEnableVertexAttribArray(0);
	// VKVertexAttribPointer(0, 3, VK_FLOAT, VK_FALSE, stride, 0);
	// // Texture coordinates
	// VKEnableVertexAttribArray(1);
	// VKVertexAttribPointer(1, 2, VK_FLOAT, VK_FALSE, stride, (void*)sizeof(glm::vec3));
	// // Normal vectors
	// VKEnableVertexAttribArray(2);
	// VKVertexAttribPointer(2, 3, VK_FLOAT, VK_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
  
  
  
}


