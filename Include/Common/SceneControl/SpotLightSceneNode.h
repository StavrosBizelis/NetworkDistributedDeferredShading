#pragma once
#include "Common/SceneControl/LightSceneNode.h"


#include "Common/Shapes/ACone.h"
#include <memory>
namespace SceneControl
{	
	/// spot light scene node - to be used in a Deferred Shading Pass
	class SpotLightSceneNode
		: public LightSceneNode
	{

		std::shared_ptr<ACone> m_coneMesh;

		float m_cutoffCos;
		float m_exponent; 

		virtual void UpdateMesh();
			
	public:
		SpotLightSceneNode(SceneNode* a_parent, std::shared_ptr<ACone> a_coneMesh);
		~SpotLightSceneNode();

		void Init();

		void SetCutoffCos(const float& a_cutoff) { m_cutoffCos = a_cutoff; m_lightDirty = true; }
		void SetCutoffRads(const float& a_rads) { SetCutoffCos(cos(a_rads)); }
		void SetCutoffDegrees(const float& a_degrees) { SetCutoffRads(a_degrees*float(0.01745329251) /* pi/180 */); }
		void SetExponent(const float& a_exponent) { m_exponent = a_exponent; m_lightDirty = true; }

		float GetCutoffCos() const {return m_cutoffCos;}
		float GetExponent() const { return m_exponent; }


		virtual void Render(glutil::MatrixStack& a_matrix = glutil::MatrixStack()) const;
		virtual void Update(const double& a_deltaTime, bool a_dirty = false, const glm::mat4& a_parentAbsoluteTrans = glm::mat4());

	};

}