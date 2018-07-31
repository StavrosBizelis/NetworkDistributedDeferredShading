#include "Vulkan/SceneControl/VKDirectionalLightSceneNode.h"

#include <iostream>

namespace SceneControl
{
  VKDirectionalLightSceneNode::VKDirectionalLightSceneNode(SceneNode* a_parent, std::shared_ptr<ARect> m_internalMesh, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry  )
  : DirectionalLightSceneNode(a_parent, m_internalMesh)
  {
   
    m_vulkanRenderOperations = new VulkanDirLightRenderable(m_internalMesh, a_updateRegistry);
  }
  
  VKDirectionalLightSceneNode::~VKDirectionalLightSceneNode()
  {
    delete m_vulkanRenderOperations;
  }
  
  void* VKDirectionalLightSceneNode::GetExtra()
  {
    return reinterpret_cast<void*>(m_vulkanRenderOperations);
  }


  
  void VKDirectionalLightSceneNode::Render(glutil::MatrixStack& a_matrix ) const{}
  
  void VKDirectionalLightSceneNode::Update(const double& a_deltaTime, bool a_dirty, const glm::mat4& a_parentAbsoluteTrans  )
  {
    bool l_dirty = m_dirty || a_dirty;
    
    DirectionalLightSceneNode::Update(a_deltaTime, a_dirty, a_parentAbsoluteTrans);
    
    if( l_dirty )
    {
      // update the local ubo structure
      glm::vec4 l_direction( 0.0f, -1.f, 0.f, 1.f);
      l_direction = GetRot() * l_direction;
      
      m_vulkanRenderOperations->m_ubo2.m_direction = l_direction;

      std::cout << "ambient is " << GetAmbient().x << " " << GetAmbient().y << " " << GetAmbient().z << std::endl;
      m_vulkanRenderOperations->m_ubo2.m_ambient = glm::vec4(GetAmbient(), 1);
      m_vulkanRenderOperations->m_ubo2.m_diffuse = glm::vec4(GetDiffuse(), 1);
      m_vulkanRenderOperations->m_ubo2.m_specular = glm::vec4(GetSpecular(), 1);
  
      m_vulkanRenderOperations->m_ubo.modelMatrix = m_lastAbsoluteTrans;
      
      // register for update
      m_vulkanRenderOperations->RegisterForUpdate();
    }
  }

  
}