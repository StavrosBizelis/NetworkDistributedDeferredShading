#pragma once
#include "Vulkan/SceneControl/VKMeshSceneNode.h"
#include <iostream>


namespace SceneControl
{
  VKMeshSceneNode::VKMeshSceneNode(SceneNode* a_parent, const std::shared_ptr<IMesh>& a_mesh, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry  )
  : MeshSceneNode(a_parent, a_mesh)
  {
   
    m_vulkanRenderOperations = new VulkanMeshRenderable(a_mesh, a_updateRegistry);
    
    m_vulkanRenderOperations->m_ubo2.UDiffuse = glm::vec4(1);
    m_vulkanRenderOperations->m_ubo2.USpecular = glm::vec4(1);
    m_vulkanRenderOperations->m_ubo2.UHardness = glm::vec4(1);
    m_vulkanRenderOperations->m_ubo2.UEmissive = glm::vec4(0);
    
    m_materialUniformsFloat.resize(4);
    m_materialUniformsVec3.resize(4);
    m_materialUniformsVec4.resize(4);
    
  }
  
  VKMeshSceneNode::~VKMeshSceneNode()
  {
   
    // in future - free the descSet
    
    delete m_vulkanRenderOperations;
  }
  
  void* VKMeshSceneNode::GetExtra()
  {
    return reinterpret_cast<void*>(m_vulkanRenderOperations);
  }
  
  void VKMeshSceneNode::Render(glutil::MatrixStack& a_matrix ) const
  {
  }
  
  void VKMeshSceneNode::Update(const double& a_deltaTime, bool a_dirty, const glm::mat4& a_parentAbsoluteTrans )
  {
    bool l_dirty = m_dirty || a_dirty;
    
    MeshSceneNode::Update(a_deltaTime, a_dirty, a_parentAbsoluteTrans);
    
    if( l_dirty )
    {
      // update the local ubo structure
      for( std::vector< std::unordered_map<std::string, glm::vec4> >::const_iterator l_iter = m_materialUniformsVec4.cbegin(); l_iter != m_materialUniformsVec4.cend(); ++l_iter)
      {
        for( std::unordered_map<std::string, glm::vec4>::const_iterator l_iter2 = l_iter->cbegin(); l_iter2 != l_iter->cend(); ++l_iter2)
        {        
          if( l_iter2->first.find("UDiffuse") != std::string::npos )
            m_vulkanRenderOperations->m_ubo2.UDiffuse = l_iter2->second;
          if( l_iter2->first.find("USpecular") != std::string::npos )
            m_vulkanRenderOperations->m_ubo2.USpecular = l_iter2->second;
          if( l_iter2->first.find("UHardness") != std::string::npos )
            m_vulkanRenderOperations->m_ubo2.UHardness = l_iter2->second;
          if( l_iter2->first.find("UEmissive") != std::string::npos )
            m_vulkanRenderOperations->m_ubo2.UEmissive = l_iter2->second;
        }
      }
      for( std::vector< std::unordered_map<std::string, glm::vec3> >::const_iterator l_iter = m_materialUniformsVec3.cbegin(); l_iter != m_materialUniformsVec3.cend(); ++l_iter)
      {
        for( std::unordered_map<std::string, glm::vec3>::const_iterator l_iter2 = l_iter->cbegin(); l_iter2 != l_iter->cend(); ++l_iter2)
        {        
          if( l_iter2->first.find("UDiffuse") != std::string::npos )
            m_vulkanRenderOperations->m_ubo2.UDiffuse = glm::vec4( l_iter2->second, 1);
          if( l_iter2->first.find("USpecular") != std::string::npos )
            m_vulkanRenderOperations->m_ubo2.USpecular = glm::vec4( l_iter2->second, 1);
          if( l_iter2->first.find("UEmissive") != std::string::npos )
            m_vulkanRenderOperations->m_ubo2.UEmissive = glm::vec4( l_iter2->second, 1);
        
        }
      }
      for( std::vector< std::unordered_map<std::string, float> >::const_iterator l_iter = m_materialUniformsFloat.cbegin(); l_iter != m_materialUniformsFloat.cend(); ++l_iter)
      {
        for( std::unordered_map<std::string, float>::const_iterator l_iter2 = l_iter->cbegin(); l_iter2 != l_iter->cend(); ++l_iter2)
        {        

          if( l_iter2->first.find("UHardness") != std::string::npos )
            m_vulkanRenderOperations->m_ubo2.UHardness = glm::vec4(l_iter2->second);
        }
      }

      
      m_vulkanRenderOperations->m_lastAbsoluteTrans = m_lastAbsoluteTrans;
      
      // register for update
      m_vulkanRenderOperations->RegisterForUpdate();
    }
  }
  
  
  
}