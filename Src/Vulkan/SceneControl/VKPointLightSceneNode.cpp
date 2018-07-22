#include "Vulkan/SceneControl/VKPointLightSceneNode.h"



namespace SceneControl
{
  VKPointLightSceneNode::VKPointLightSceneNode(SceneNode* a_parent, std::shared_ptr<ASphere> a_sphere, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry  )
  : PointLightSceneNode(a_parent, a_sphere), VulkanRenderable(a_updateRegistry)
  {
    m_uniformBuffer = nullptr;
    m_descSet = NULL;
  }
  
  VKPointLightSceneNode::~VKPointLightSceneNode()
  {
    if( m_uniformBuffer )
      m_uniformBuffer->Free();
    // in future - free the descSet
  }
  
  void VKPointLightSceneNode::Init(const VkDescriptorSet& a_descSet, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer2)
  {
    m_descSet = a_descSet;
    m_uniformBuffer = a_uniformBuffer;
    m_uniformBuffer2 = a_uniformBuffer2;
  }
  
  void VKPointLightSceneNode::Render(glutil::MatrixStack& a_matrix ) const{}
  
  void VKPointLightSceneNode::Update(const double& a_deltaTime, bool a_dirty, const glm::mat4& a_parentAbsoluteTrans )
  {
    bool l_dirty = m_dirty || a_dirty;
    
    PointLightSceneNode::Update(a_deltaTime, a_dirty, a_parentAbsoluteTrans);
    
    if( l_dirty )
    {
      // update the local ubo structure
      m_ubo2.m_position =  GetPos();

      m_ubo2.m_diffuse = GetDiffuse();
      m_ubo2.m_specular = GetSpecular();
  
      m_ubo2.m_constantAtt = GetConstAttenuation();
      m_ubo2.m_linearAtt = GetLinAttenuation();
      m_ubo2.m_quadraticAtt = GetQuadAttenuation();
      
      // register for update
      RegisterForUpdate();
    }
  }
  
  void VKPointLightSceneNode::VulkanUpdate(char* a_mappedUBO)
  {
    // map the model matrix
    memcpy(a_mappedUBO+m_uniformBuffer->GetMemoryOffset(), &m_lastAbsoluteTrans, sizeof(VertexObjectMatrices));
    // map the point light data
    memcpy(a_mappedUBO+m_uniformBuffer2->GetMemoryOffset(), &m_ubo2, sizeof(FragPointLight) );
  }
  
  
}