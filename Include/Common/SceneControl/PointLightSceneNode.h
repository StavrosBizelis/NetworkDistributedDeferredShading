#pragma once
#include "Common/SceneControl/LightSceneNode.h"
#include "Common/Shapes/ASphere.h"
namespace SceneControl
{
	/// point light scene node - to be used in a Deferred Shading Pass
	class PointLightSceneNode
		: public LightSceneNode
	{

		// internal mesh vars
		std::shared_ptr<ASphere> m_sphere;

		virtual void UpdateMesh();
			
	public:
		PointLightSceneNode(SceneNode* a_parent, std::shared_ptr<ASphere> a_sphere );
		~PointLightSceneNode();

		void Init();


		virtual void Render(glutil::MatrixStack& a_matrix = glutil::MatrixStack()) const;
		virtual void Update(const double& a_deltaTime, bool a_dirty = false, const glm::mat4& a_parentAbsoluteTrans = glm::mat4());

	};

}