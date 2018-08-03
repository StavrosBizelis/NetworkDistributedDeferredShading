#include "Common/SceneControl/SpotLightSceneNode.h"
#include <algorithm> 
namespace SceneControl
{

	SpotLightSceneNode::SpotLightSceneNode(SceneNode* a_parent, std::shared_ptr<ACone> a_coneMesh)
		: LightSceneNode(a_parent), m_coneMesh(a_coneMesh), m_cutoffCos(0.70710678118f), m_exponent(0) {}


	SpotLightSceneNode::~SpotLightSceneNode()
	{
	}

	void SpotLightSceneNode::Init(){}





	void SpotLightSceneNode::Render(glutil::MatrixStack & a_matrix) const
	{
		if (!GetEnabled())
			return;

		// std::shared_ptr<IShaderProgram> l_material0 = GetMaterial(1);	// null material
		std::shared_ptr<IShaderProgram> l_material1 = GetMaterial(0);	// light material
		// cannot render without the proper materials
		// if (!l_material0 || !l_material1)
		if (!l_material1)
			return;



		// apply absolute tranformation and push it in the 
		a_matrix.Push();
		a_matrix.ApplyMatrix(m_lastAbsoluteTrans);

		// do the stencil test pass
		// // // // // // // // // // // // // // l_material0->UseProgram();
		// // // // // // // // // // // // // // l_material0->SetUniform("matrices.projModelViewMatrixStack", a_matrix.Top());

		// // // // // // // // // // // // // // // render light mesh for stencil pass
		// // // // // // // // // // // // // // m_coneMesh->Render();





		// do the light pass
		l_material1->UseProgram();
		l_material1->SetUniform("matrices.projModelViewMatrixStack", a_matrix.Top());
		//l_material1->SetUniform("UInverseViewProjectionMatrix", glm::inverse( a_matrix.Top() ) );

		glm::vec3 l_pos(
			m_lastAbsoluteTrans[3][0],
			m_lastAbsoluteTrans[3][1],
			m_lastAbsoluteTrans[3][2]
			);
		l_material1->SetUniform("ULightData.m_position", l_pos);
		// set the light state to the material ( attenuation, colour )
		l_material1->SetUniform("ULightData.m_constantAtt", m_constantAtt);
		l_material1->SetUniform("ULightData.m_linearAtt", m_linearAtt);
		l_material1->SetUniform("ULightData.m_quadraticAtt", m_quadraticAtt);

		l_material1->SetUniform("ULightData.m_ambient", m_ambCol);
		l_material1->SetUniform("ULightData.m_diffuse", m_difCol);
		l_material1->SetUniform("ULightData.m_specular", m_specCol);


		glm::vec4 l_direction(0.0f, -1.f, 0.f, 1.f);
		l_direction = GetRot() * l_direction;
		l_material1->SetUniform("ULightData.m_direction", glm::normalize(glm::vec3(l_direction) ) );

		l_material1->SetUniform("ULightData.m_spotCutoff", m_cutoffCos);
		l_material1->SetUniform("ULightData.m_spotExponent", m_exponent);
		

		// render light mesh for light pass
		m_coneMesh->Render();
		a_matrix.Pop();
		
	}

	void SpotLightSceneNode::Update(const double & a_deltaTime, bool a_dirty, const glm::mat4 & a_parentAbsoluteTrans)
	{
		if (!GetEnabled())
			return;

		if (m_lightDirty)
			UpdateMesh();
		SceneNode::Update(a_deltaTime, a_dirty, a_parentAbsoluteTrans);
	}

	void SpotLightSceneNode::UpdateMesh()
	{

		float l_rads = acos(m_cutoffCos);
		float l_tangent = float( tan(l_rads) * 1.1);
		// Tangent == Opposite / Adjacent => Opposite == Tangent * adjacent, but if Adjacent is 1 so Opposite == Tangent


		float l_maxChannel = std::max(std::max(m_difCol.r, m_difCol.g), m_difCol.b);
		float l_intensity = 0.2126f * m_difCol.x + 0.7152f * m_difCol.y + 0.0722f * m_difCol.z;;// (m_difCol.r + m_difCol.g + m_difCol.b) ;
		float ret = (-m_linearAtt + sqrtf( pow(m_linearAtt, 2) - 4 * m_quadraticAtt * (m_constantAtt - (256.f/3.f) * l_maxChannel * l_intensity/*intensity*/)) )
			/
			float(2 * m_quadraticAtt);

		if (ret > 0 )
			SetScale( glm::vec3(l_tangent*ret, ret, l_tangent*ret) );
		else
			SetScale(glm::vec3(.1));

		m_lightDirty = false;
	}

}