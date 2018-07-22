#pragma once
#include "Common/SceneControl/LightSceneNode.h"

#include "Common/Shapes/ARect.h"

namespace SceneControl
{
	/// directional light scene node - to be used in a Deferred Shading Pass
	class DirectionalLightSceneNode
		: public LightSceneNode
	{
    protected:
		// internal mesh vars
		std::shared_ptr<ARect> m_fullScreenRect;


    public:
		DirectionalLightSceneNode(SceneNode* a_parent, std::shared_ptr<ARect> m_internalMesh);
		~DirectionalLightSceneNode();

		void Init();


		virtual void Render(glutil::MatrixStack& a_matrix = glutil::MatrixStack()) const;
		virtual void Update(const double& a_deltaTime, bool a_dirty = false, const glm::mat4& a_parentAbsoluteTrans = glm::mat4());

	};

}