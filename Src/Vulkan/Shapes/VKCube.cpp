/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKCube.cpp
 *   DATE: Fri Jul 13 19:30:00 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/Shapes/VKCube.h"

/*
 *  Method: VKCube::VKCube
 *  Params: 
 * Effects: 
 */
VKCube::VKCube(const std::shared_ptr<VulkanMemory>& a_memory) :ACube(), m_memory(a_memory), m_vulkanShape(nullptr)
{
}


/*
 *  Method: VKCube::~VKCube
 *  Params: 
 * Effects: 
 */
VKCube::~VKCube()
{
  Release();
}


/*
 *  Method: VKCube::Render
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKCube::Render()
{
  // VKBindVertexArray(m_vao);
	// VKDrawArrays(VK_TRIANVKE_STRIP, 0, 4);
	// VKDrawArrays(VK_TRIANVKE_STRIP, 4, 4);
	// VKDrawArrays(VK_TRIANVKE_STRIP, 8, 4);
	// VKDrawArrays(VK_TRIANVKE_STRIP, 12, 4);
	// VKDrawArrays(VK_TRIANVKE_STRIP, 16, 4);
	// VKDrawArrays(VK_TRIANVKE_STRIP, 20, 4);
}


/*
 *  Method: VKCube::Release
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKCube::Release()
{
  if( m_vulkanShape )
    delete m_vulkanShape;
}


/*
 *  Method: VKCube::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKCube::Create()
{
  ACube::Create();

  
  // use this only for vk version
  std::vector<unsigned int> l_indices = {0,1,2, 2,1,3, 
                                         4,5,6, 6,5,7, 
                                         8,9,10, 10,9,11, 
                                         12,13,14, 14,13,15, 
                                         16,17,18, 18,17,19, 
                                         20,21,22, 22,21,23 };
  // upload data to the gpu
  
  std::shared_ptr<VulkanMemoryChunk> l_vkVertices = m_memory->CreateVertexBuffer( (char*)m_vertices.data(), m_vertices.size() * sizeof(Vertex) );
  std::shared_ptr<VulkanMemoryChunk> l_vkIndices = m_memory->CreateIndexBuffer( (char*)l_indices.data(), l_indices.size() * sizeof(unsigned int) );
  
  m_vulkanShape = new VKShape(m_memory, {l_vkVertices}, {l_vkIndices}, {l_indices.size()});
  
  // // Set the vertex attribute locations
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


