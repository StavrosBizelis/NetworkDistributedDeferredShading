#pragma once
#include "Common/SceneControl/SpotLightSceneNode.h"
#include "Vulkan/RenderControl/VulkanRenderable.h"
#include "Vulkan/Shapes/VKCone.h"
#include "Vulkan/RenderControl/UniformBufferObjects.h"
namespace SceneControl
{
	/// point light scene node - to be used in a Deferred Shading Pass
	class VKSpotLightSceneNode
		: public SpotLightSceneNode
	{

		// internal mesh vars

    std::shared_ptr<VulkanMemoryChunk> m_uniformBuffer;
    std::shared_ptr<VulkanMemoryChunk> m_uniformBuffer2;
    VkDescriptorSet m_descSet;
    
    VertexObjectMatrices m_ubo;
    FragPointLight m_ubo2;
    
    VulkanSpotLightRenderable* m_vulkanRenderOperations;
    
		// virtual void UpdateMesh();
			
	public:
		VKSpotLightSceneNode(SceneNode* a_parent, std::shared_ptr<ACone> a_cone, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry);
		~VKSpotLightSceneNode();
  
    virtual void* GetExtra();
    
    
		virtual void Render(glutil::MatrixStack& a_matrix = glutil::MatrixStack()) const;
		virtual void Update(const double& a_deltaTime, bool a_dirty = false, const glm::mat4& a_parentAbsoluteTrans = glm::mat4());

	};

}