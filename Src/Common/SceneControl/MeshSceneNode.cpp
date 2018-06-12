#include "Common/SceneControl/MeshSceneNode.h"

namespace SceneControl
{

	MeshSceneNode::MeshSceneNode(SceneNode* a_parent, const std::shared_ptr<IMesh>& a_mesh)
		: TexturedSceneNode(a_parent), m_mesh(a_mesh) 
	{
//		std::shared_ptr<CTexture> l_texture = m_mesh->GetTexture();
//		if (l_texture)
//			SetTexture(0, l_texture);
	}


	MeshSceneNode::~MeshSceneNode()
	{
	}


	void MeshSceneNode::Render(glutil::MatrixStack& a_matrix) const
	{
		if (!GetEnabled())
			return;
		std::shared_ptr<IShaderProgram> l_material0 = GetMaterial(0);
		// cannot render without a material
		if (!l_material0)
			return;
			
		

		BindTextures();
		l_material0->UseProgram();

		BindPersistentUniforms(0);

		a_matrix.Push();
		// apply absolute tranformation and push it in the material
		a_matrix.ApplyMatrix(m_lastAbsoluteTrans);
		l_material0->SetUniform("matrices.modelViewMatrix", a_matrix.Top());
		l_material0->SetUniform("matrices.modelMatrix", m_lastAbsoluteTrans);

		if(m_absoluteScale.x == m_absoluteScale.y && m_absoluteScale.x == m_absoluteScale.z)
			l_material0->SetUniform("matrices.normalMatrix", glm::mat3(m_lastAbsoluteTrans));
		else
			l_material0->SetUniform("matrices.normalMatrix", glm::transpose(glm::inverse( glm::mat3( m_lastAbsoluteTrans ) )) );
		a_matrix.Pop();
	
		m_mesh->Render();
	}

}