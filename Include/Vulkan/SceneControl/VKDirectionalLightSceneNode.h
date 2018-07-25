#pragma once
#include "Common/SceneControl/DirectionalLightSceneNode.h"
#include "Vulkan/RenderControl/VulkanRenderable.h"

#include "Vulkan/Shapes/VKRect.h"
#include "Vulkan/RenderControl/UniformBufferObjects.h"



namespace SceneControl
{
	/// directional light scene node - to be used in a Deferred Shading Pass
	class VKDirectionalLightSceneNode
		: public DirectionalLightSceneNode
	{

		// internal mesh vars
		std::shared_ptr<VKRect> m_fullScreenRect;
    
    VulkanDirLightRenderable* m_vulkanRenderOperations;

	public:
		VKDirectionalLightSceneNode(SceneNode* a_parent, std::shared_ptr<ARect> m_internalMesh, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry);
		~VKDirectionalLightSceneNode();
    
    virtual void* GetExtra(); 


		virtual void Render(glutil::MatrixStack& a_matrix = glutil::MatrixStack()) const;
		virtual void Update(const double& a_deltaTime, bool a_dirty = false, const glm::mat4& a_parentAbsoluteTrans = glm::mat4());
    
	};

}