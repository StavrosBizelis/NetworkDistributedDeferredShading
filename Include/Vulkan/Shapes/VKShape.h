#pragma once
#include <memory>
#include "Vulkan/Core/VulkanMemory.h"

class VKShape
{
  protected:
  std::shared_ptr<VulkanMemory> m_memory;
  std::vector<std::shared_ptr<VulkanMemoryChunk> > m_vkVertices;
  std::vector<std::shared_ptr<VulkanMemoryChunk> > m_vkIndices;
  std::vector<unsigned int> m_indicesCount;
  
  public:
  
  VKShape(std::shared_ptr<VulkanMemory> a_memory, const std::vector<std::shared_ptr<VulkanMemoryChunk> >& a_vkVertices, 
          const std::vector<std::shared_ptr<VulkanMemoryChunk> >& a_vkIndices, const std::vector<unsigned int>& a_indicesCount)
          :m_memory(a_memory), m_vkVertices(a_vkVertices), m_vkIndices(a_vkIndices), m_indicesCount(a_indicesCount) {}
          
  ~VKShape()
  {
    for(unsigned int i = 0; i < m_vkVertices.size(); ++i)
    {
      if( m_vkVertices[i] )
        m_vkVertices[i]->Free();
    }
    
    for(unsigned int i = 0; i < m_vkIndices.size(); ++i)
    {
      if( m_vkIndices[i] )
        m_vkIndices[i]->Free();
    }
  }
  
  virtual unsigned int GetMeshesCount(){ return m_indicesCount.size(); }
  virtual std::shared_ptr<VulkanMemoryChunk> GetVertices(const unsigned int& a_index = 0)
  {
    if( a_index < m_vkVertices.size() )
      return m_vkVertices[a_index];
  }
  virtual std::shared_ptr<VulkanMemoryChunk> GetIndices(const unsigned int& a_index = 0)
  {
    if( a_index < m_vkIndices.size() )
      return m_vkIndices[a_index];    
  }
  virtual unsigned int GetIndicesCount(const unsigned int& a_index = 0)
  {
    if( a_index < m_indicesCount.size() )
      return m_indicesCount[a_index];    
  }
};