#include "Common/SceneControl/DirectionalLightSceneNode.h"
#include <algorithm> 
namespace SceneControl
{

	DirectionalLightSceneNode::DirectionalLightSceneNode(SceneNode* a_parent, std::shared_ptr<ARect> m_internalMesh)
		: LightSceneNode(a_parent)
	{
		m_fullScreenRect = m_internalMesh;
	}


	DirectionalLightSceneNode::~DirectionalLightSceneNode()
	{
	}

	void DirectionalLightSceneNode::Init(){}





	void DirectionalLightSceneNode::Render(glutil::MatrixStack & a_matrix) const
	{
		if (!GetEnabled())
			return;
		std::shared_ptr<IShaderProgram> l_material0 = GetMaterial(0);	// light material
																		// cannot render without the proper materials
		if (!l_material0 )
			return;



		// apply absolute tranformation and push it in the 
		a_matrix.Push();

		// do the light pass
		l_material0->UseProgram();
		// l_material0->SetUniform("UInverseViewProjectionMatrix", glm::inverse(a_matrix.Top()));

		// set the light state to the material ( attenuation, colour )
		l_material0->SetUniform("ULightData.m_ambient", m_ambCol);
		l_material0->SetUniform("ULightData.m_diffuse", m_difCol);
		l_material0->SetUniform("ULightData.m_specular", m_specCol);


		glm::vec4 l_direction( 0.0f, -1.f, 0.f, 1.f);
		l_direction = GetRot() * l_direction;
		l_material0->SetUniform("ULightData.m_direction", glm::vec3( l_direction ) );

		m_fullScreenRect->Render();
		a_matrix.Pop();


	}

	void DirectionalLightSceneNode::Update(const double & a_deltaTime, bool a_dirty, const glm::mat4 & a_parentAbsoluteTrans)
	{

		SceneNode::Update(a_deltaTime, a_dirty, a_parentAbsoluteTrans);
	}



}