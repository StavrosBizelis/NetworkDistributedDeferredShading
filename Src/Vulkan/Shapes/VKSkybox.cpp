/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKSkybox.cpp
 *   DATE: Fri Jul 13 19:30:00 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/Shapes/VKSkybox.h"

/*
 *  Method: VKSkybox::VKSkybox
 *  Params: 
 * Effects: 
 */
VKSkybox::VKSkybox(const std::shared_ptr<VulkanMemory>& a_memory) : ASkybox(), m_memory(a_memory), m_vkVertices(nullptr), m_vkIndices(nullptr)
{
}


/*
 *  Method: VKSkybox::~VKSkybox
 *  Params: 
 * Effects: 
 */
VKSkybox::~VKSkybox()
{
  Release();
}


/*
 *  Method: VKSkybox::Render
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKSkybox::Render()
{
  // //VKDepthMask(0);
	// VKBindVertexArray(m_vao);
	// //m_cubemapTexture.Bind(textureUnit);
	// for (int i = 0; i < 6; i++) {
	// //	//m_textures[i].Bind();
		// VKDrawArrays(VK_TRIANVKE_STRIP, i*4, 4);
	// }
	// //VKDepthMask(1);
}


/*
 *  Method: VKSkybox::Release
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKSkybox::Release()
{
  if( m_vkVertices )
    m_vkVertices->Free();
  if( m_vkIndices )
    m_vkIndices->Free();
}


/*
 *  Method: VKSkybox::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void
VKSkybox::Create()
{
  ASkybox::Create();
  
  // use this only for vk version
  std::vector<unsigned int> l_indices = {0,1,2, 2,1,3, 
                                         4,5,6, 6,5,7, 
                                         8,9,10, 10,9,11, 
                                         12,13,14, 14,13,15, 
                                         16,17,18, 18,17,19, 
                                         20,21,22, 22,21,23 };
  
  // upload data to the gpu
  m_vkVertices = m_memory->CreateVertexBuffer( (char*)m_vertices.data(), m_vertices.size() * sizeof(Vertex) );
  m_vkIndices = m_memory->CreateIndexBuffer( (char*)l_indices.data(), l_indices.size() * sizeof(unsigned int) );
  

	// // Set the vertex attribute locations
	// VKsizei istride = 2*sizeof(glm::vec3)+sizeof(glm::vec2);

	// // Vertex positions
	// VKEnableVertexAttribArray(0);
	// VKVertexAttribPointer(0, 3, VK_FLOAT, VK_FALSE, istride, 0);
	// // Texture coordinates
	// VKEnableVertexAttribArray(1);
	// VKVertexAttribPointer(1, 2, VK_FLOAT, VK_FALSE, istride, (void*)sizeof(glm::vec3));
	// // Normal vectors
	// VKEnableVertexAttribArray(2);
	// VKVertexAttribPointer(2, 3, VK_FLOAT, VK_FALSE, istride, (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));
  
}


