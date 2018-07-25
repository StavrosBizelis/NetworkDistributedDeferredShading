/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKCone.cpp
 *   DATE: Fri Jul 13 19:30:00 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/Shapes/VKCone.h"

/*
 *  Method: VKCone::VKCone
 *  Params: 
 * Effects: 
 */
VKCone::VKCone(const std::shared_ptr<VulkanMemory>& a_memory) : ACone(), m_memory(a_memory), m_vulkanShape(nullptr)
{
}


/*
 *  Method: VKCone::~VKCone
 *  Params: 
 * Effects: 
 */
VKCone::~VKCone()
{
  Release();
}


/*
 *  Method: VKCone::Render
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKCone::Render()
{
  // VKBindVertexArray(m_vao);
	// VKEnableVertexAttribArray(0);
	// m_vbo.Bind();

	// VKDrawElements(VK_TRIANVKES, m_numTrianVKes * 3, VK_UNSIGNED_INT, 0);

	// VKDisableVertexAttribArray(0);
}


/*
 *  Method: VKCone::Release
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKCone::Release()
{
  if( m_vulkanShape )
    delete m_vulkanShape;
}


/*
 *  Method: VKCone::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKCone::Create()
{
  ACone::Create();

  // upload data to the gpu
  std::shared_ptr<VulkanMemoryChunk> l_vkVertices = m_memory->CreateVertexBuffer( (char*)m_vertices.data(), m_vertices.size() * sizeof(Vertex) );
  std::shared_ptr<VulkanMemoryChunk> l_vkIndices = m_memory->CreateIndexBuffer( (char*)m_indices.data(), m_indices.size() * sizeof(unsigned int) );
  
  m_vulkanShape = new VKShape(m_memory, {l_vkVertices}, {l_vkIndices}, {m_indices.size()});

	// VKsizei stride = sizeof(glm::vec3);
	// // Vertex positions
	// VKEnableVertexAttribArray(0);
	// VKVertexAttribPointer(0, 3, VK_FLOAT, VK_FALSE, stride, 0);
  
}


