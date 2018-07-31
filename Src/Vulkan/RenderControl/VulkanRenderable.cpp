#include "Vulkan/RenderControl/VulkanRenderable.h"



VulkanRenderable::VulkanRenderable(std::shared_ptr<IMesh> a_shape, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry)
: m_updateRegistry(a_updateRegistry), m_shape(a_shape), m_uniformBuffer(nullptr), m_uniformBuffer2(nullptr){}

VulkanRenderable::~VulkanRenderable()
{
  if( m_uniformBuffer )
    m_uniformBuffer->Free();
  
  if( m_uniformBuffer2 )
    m_uniformBuffer2->Free();
}

void VulkanRenderable::Init(const VkDescriptorSet& a_descSet, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer1, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer2 )
{
  
  m_descSet = a_descSet;
  m_uniformBuffer = a_uniformBuffer1;
  m_uniformBuffer2 = a_uniformBuffer2;
}

// needed for the actual command recording
std::shared_ptr<IMesh> VulkanRenderable::GetShape(){return m_shape;}
void VulkanRenderable::SetDesciptorSet(const VkDescriptorSet& a_descSet){m_descSet = a_descSet;} ///< pointer to a descriptor set because it can be non existant
VkDescriptorSet* VulkanRenderable::GetDesciptorSet(){return &m_descSet;} ///< pointer to a descriptor set because it can be non existant


void VulkanRenderable::RegisterForUpdate()
{
  if( m_updateRegistry )
    m_updateRegistry->push_back(this);
}

std::shared_ptr< std::vector<VulkanRenderable*> > VulkanRenderable::GetUpdateRegistry() {return m_updateRegistry; }
  



VulkanMeshRenderable::VulkanMeshRenderable(std::shared_ptr<IMesh> a_shape, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry)
  : VulkanRenderable(a_shape, a_updateRegistry){}
  
void VulkanMeshRenderable::VulkanUpdate(char* a_mappedUBO)
{
  if( m_uniformBuffer )
  {
    // map the model matrix
    memcpy(a_mappedUBO+m_uniformBuffer->GetMemoryOffset(), &m_lastAbsoluteTrans, sizeof(VertexObjectMatrices));
  }
  if( m_uniformBuffer2 )
  {
    // map the material data
    memcpy(a_mappedUBO+m_uniformBuffer2->GetMemoryOffset(), &m_ubo2, sizeof(FragMaterialData));
  }
}


VulkanDirLightRenderable::VulkanDirLightRenderable(std::shared_ptr<IMesh> a_shape, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry)
  : VulkanRenderable(a_shape, a_updateRegistry){}
  
void VulkanDirLightRenderable::VulkanUpdate(char* a_mappedUBO)
{
  if( m_uniformBuffer )
  {
    // map the model matrix
    memcpy(a_mappedUBO+m_uniformBuffer->GetMemoryOffset(), &m_ubo, sizeof(VertexObjectMatrices));
  }
  if( m_uniformBuffer2 )
  {
    // map the material data
    memcpy(a_mappedUBO+m_uniformBuffer2->GetMemoryOffset(), &m_ubo2, sizeof(FragDirectionalLight));
  }
}


VulkanPointLightRenderable::VulkanPointLightRenderable(std::shared_ptr<IMesh> a_shape, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry)
  : VulkanRenderable(a_shape, a_updateRegistry){}
  
void VulkanPointLightRenderable::VulkanUpdate(char* a_mappedUBO)
{
  if( m_uniformBuffer )
  {
    // map the model matrix
    memcpy(a_mappedUBO+m_uniformBuffer->GetMemoryOffset(), &m_ubo, sizeof(VertexObjectMatrices));
  }
  if( m_uniformBuffer2 )
  {
    // map the material data
    memcpy(a_mappedUBO+m_uniformBuffer2->GetMemoryOffset(), &m_ubo2, sizeof(FragPointLight));
  }
}


