#include "Vulkan/SceneControl/VKSpotLightSceneNode.h"



namespace SceneControl
{
  VKSpotLightSceneNode::VKSpotLightSceneNode(SceneNode* a_parent, std::shared_ptr<ACone> a_cone, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry  )
  : SpotLightSceneNode(a_parent, a_cone)
  {
    m_vulkanRenderOperations = new VulkanSpotLightRenderable(a_cone, a_updateRegistry);
  }
  
  VKSpotLightSceneNode::~VKSpotLightSceneNode()
  {
    delete m_vulkanRenderOperations;
  }
  
  void* VKSpotLightSceneNode::GetExtra()
  {
    return reinterpret_cast<void*>(m_vulkanRenderOperations);
  }

  void VKSpotLightSceneNode::Render(glutil::MatrixStack& a_matrix ) const{}
  
  void VKSpotLightSceneNode::Update(const double& a_deltaTime, bool a_dirty, const glm::mat4& a_parentAbsoluteTrans )
  {
    bool l_dirty = m_dirty || a_dirty;
    
    SpotLightSceneNode::Update(a_deltaTime, a_dirty, a_parentAbsoluteTrans);
    
    if( l_dirty )
    {
      // update the local ubo structure
      m_vulkanRenderOperations->m_ubo2.m_position =  glm::vec4(GetPos() , 1);

      
      glm::vec4 l_direction( 0.0f, -1.f, 0.f, 1.f);
      l_direction = GetRot() * l_direction;
      m_vulkanRenderOperations->m_ubo2.m_direction = glm::normalize(l_direction);
      
      
      
      m_vulkanRenderOperations->m_ubo2.m_spotCutoffAndExponent = glm::vec4( GetCutoffCos(), GetExponent(), 0, 0);

      m_vulkanRenderOperations->m_ubo2.m_diffuse = glm::vec4(GetDiffuse(), 1);
      m_vulkanRenderOperations->m_ubo2.m_specular = glm::vec4(GetSpecular(), 1);
      
      m_vulkanRenderOperations->m_ubo2.m_attenuation = glm::vec4( GetConstAttenuation(), GetLinAttenuation(), GetQuadAttenuation(), 1 );
      
      std::cout << "position :" << GetPos().x<< " " << GetPos().y << " " << GetPos().z << std::endl; 
      std::cout << "direction :" << l_direction.x<< " " << l_direction.y << " " << l_direction.z << std::endl; 
      std::cout << "cutoff, exponent :" << GetCutoffCos() << " " << GetExponent() << std::endl; 
      
      m_vulkanRenderOperations->m_ubo.modelMatrix = m_lastAbsoluteTrans;
      
      // register for update
      m_vulkanRenderOperations->RegisterForUpdate();
    }
  }
  

  
  
}