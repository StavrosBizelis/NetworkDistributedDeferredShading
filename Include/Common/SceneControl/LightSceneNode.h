#pragma once
#include "Common/SceneControl/SceneNode.h"
namespace SceneControl
{
	/// Abstract Light Class - dont use it as it is. Inherit from it and implement a light mesh
	class LightSceneNode :
		public SceneNode
	{
	protected:
		// attenuation values
		float m_constantAtt;
		float m_linearAtt;
		float m_quadraticAtt;

		glm::vec3 m_ambCol;
		glm::vec3 m_difCol;
		glm::vec3 m_specCol;

		bool m_lightDirty;
	public:
		LightSceneNode(SceneNode* a_parent) : SceneNode( a_parent ), m_lightDirty(true), m_constantAtt(0), m_linearAtt(0), m_quadraticAtt(1), m_ambCol(0.f), m_difCol(1.f), m_specCol(1.f) {};
		virtual ~LightSceneNode() {};


		// setters
		inline void SetAmbient(const glm::vec3& a_col) { m_ambCol = a_col; }
		inline void SetDiffuse(const glm::vec3& a_col) { m_difCol = a_col; m_lightDirty = true; }
		inline void SetSpecular(const glm::vec3& a_col) { m_specCol = a_col; }

		inline void SetConstAttenuation(const float& a_att) { m_constantAtt = a_att; m_lightDirty = true;}
		inline void SetLinAttenuation(const float& a_att) { m_linearAtt = a_att; m_lightDirty = true;}
		inline void SetQuadAttenuation(const float& a_att) { m_quadraticAtt = a_att; m_lightDirty = true;}

		// getters
		inline glm::vec3 GetAmbient() const { return m_ambCol; }
		inline glm::vec3 GetDiffuse() const { return m_difCol; }
		inline glm::vec3 GetSpecular() const { return m_specCol; }

		inline float GetConstAttenuation() const { return m_constantAtt; }
		inline float GetLinAttenuation() const { return m_linearAtt; }
		inline float GetQuadAttenuation() const { return m_quadraticAtt; }

	};

}