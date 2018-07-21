#pragma once
#include "Common/SceneControl/DirectionalLightSceneNode.h"
#include "Vulkan/RenderControl/VulkanRenderable.h"

#include "Vulkan/Shapes/VKRect.h"
#include "Vulkan/RenderControl/UniformBufferObjects.h"



namespace SceneControl
{
	/// directional light scene node - to be used in a Deferred Shading Pass
	class VKDirectionalLightSceneNode
		: public DirectionalLightSceneNode, public VulkanRenderable
	{

		// internal mesh vars
		std::shared_ptr<VKRect> m_fullScreenRect;
    
    std::shared_ptr<VulkanMemoryChunk> m_uniformBuffer;
    std::shared_ptr<VulkanMemoryChunk> m_uniformBuffer2;
    VkDescriptorSet m_descSet;
    VertexObjectMatrices m_ubo;
    FragDirectionalLight m_ubo2;

	public:
		VKDirectionalLightSceneNode(SceneNode* a_parent, std::shared_ptr<ARect> m_internalMesh);
		~VKDirectionalLightSceneNode();

		virtual void Init(const VkDescriptorSet& a_descSet, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer2 );


		virtual void Render(glutil::MatrixStack& a_matrix = glutil::MatrixStack()) const;
		virtual void Update(const double& a_deltaTime, bool a_dirty = false, const glm::mat4& a_parentAbsoluteTrans = glm::mat4());

    
    virtual VkDescriptorSet* GetDesciptorSet(){return &m_descSet;}
    virtual std::shared_ptr<VKShape> GetShape(){ return m_sphere;}
    virtual std::shared_ptr<VulkanMemoryChunk> GetUniformBuffer(){return m_uniformBuffer;}
    virtual std::shared_ptr<VKATexture> GetVKTexture(const unsigned int& a_index){ return nullptr; }
    virtual void VulkanUpdate(char* a_mappedUBO);

	};

}