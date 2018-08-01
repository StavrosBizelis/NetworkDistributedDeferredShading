#include "Vulkan/SceneControl/VKPointLightSceneNode.h"



namespace SceneControl
{
  VKPointLightSceneNode::VKPointLightSceneNode(SceneNode* a_parent, std::shared_ptr<ASphere> a_sphere, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry  )
  : PointLightSceneNode(a_parent, a_sphere)
  {
    m_vulkanRenderOperations = new VulkanPointLightRenderable(a_sphere, a_updateRegistry);
  }
  
  VKPointLightSceneNode::~VKPointLightSceneNode()
  {
    delete m_vulkanRenderOperations;
  }
  
  void* VKPointLightSceneNode::GetExtra()
  {
    return reinterpret_cast<void*>(m_vulkanRenderOperations);
  }

  void VKPointLightSceneNode::Render(glutil::MatrixStack& a_matrix ) const{}
  
  void VKPointLightSceneNode::Update(const double& a_deltaTime, bool a_dirty, const glm::mat4& a_parentAbsoluteTrans )
  {
    bool l_dirty = m_dirty || a_dirty;
    
    PointLightSceneNode::Update(a_deltaTime, a_dirty, a_parentAbsoluteTrans);
    
    if( l_dirty )
    {
      // update the local ubo structure
      m_vulkanRenderOperations->m_ubo2.m_position =  glm::vec4(GetPos() , 1);

      m_vulkanRenderOperations->m_ubo2.m_diffuse = glm::vec4(GetDiffuse(), 1);
      m_vulkanRenderOperations->m_ubo2.m_specular = glm::vec4(GetSpecular(), 1);
      
      m_vulkanRenderOperations->m_ubo2.m_attenuation = glm::vec4( GetConstAttenuation(), GetLinAttenuation(), GetQuadAttenuation(), 1 );
      
      m_vulkanRenderOperations->m_ubo.modelMatrix = m_lastAbsoluteTrans;
      
      // register for update
      m_vulkanRenderOperations->RegisterForUpdate();
    }
  }
  

  
  
}