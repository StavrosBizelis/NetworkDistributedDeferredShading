#include "Common/RenderControl/ADeferredShadingPass.h"
#include <memory>


namespace RenderControl
{
	ADeferredShadingPass::ADeferredShadingPass(const glm::vec2& a_resolution)
		:IRenderPass(a_resolution){}


	ADeferredShadingPass::~ADeferredShadingPass()
	{}

	bool ADeferredShadingPass::AddRenderable(IRenderable * a_renderable, const GeometryPassMaterialFlags& a_geometryMaterialFlags)
	{
		if (!Exists(a_renderable))
		{
			std::shared_ptr<IShaderProgram> l_selectedMat;
			// get the appropriate material func
			// simple color
			if ((a_geometryMaterialFlags & (NORMAL_MAP | DIFFUSE_MAP | SPECULAR_MAP | HARDNESS_MAP | EMISSION_MAP)) == (NORMAL_MAP | DIFFUSE_MAP | SPECULAR_MAP | HARDNESS_MAP | EMISSION_MAP))
				l_selectedMat = GetMaterialManager()->GetColourNormalSpecHardnessEmissionGeometryPassMaterial("", "");
			else if ((a_geometryMaterialFlags & (NORMAL_MAP | DIFFUSE_MAP | SPECULAR_MAP | HARDNESS_MAP )) == (NORMAL_MAP | DIFFUSE_MAP | SPECULAR_MAP | HARDNESS_MAP))
				l_selectedMat = GetMaterialManager()->GetColourNormalSpecHardnessGeometryPassMaterial("", "");
			else if ((a_geometryMaterialFlags & (NORMAL_MAP | DIFFUSE_MAP | SPECULAR_MAP)) == (NORMAL_MAP | DIFFUSE_MAP | SPECULAR_MAP))
				l_selectedMat = GetMaterialManager()->GetColourNormalSpecGeometryPassMaterial("", "");
			else if ((a_geometryMaterialFlags & (NORMAL_MAP | DIFFUSE_MAP)) == (NORMAL_MAP | DIFFUSE_MAP))
				l_selectedMat = GetMaterialManager()->GetColourNormalGeometryPassMaterial("", "");
			else if ((a_geometryMaterialFlags & (DIFFUSE_MAP)) == (DIFFUSE_MAP))
				l_selectedMat = GetMaterialManager()->GetColourGeometryPassMaterial("", "");
			else if ((a_geometryMaterialFlags & (EMISSION_MAP)) == (EMISSION_MAP))
				l_selectedMat = GetMaterialManager()->GetEmissiveGeometryPassMaterial("", "");
			else if ((a_geometryMaterialFlags & (SKYBOX)) == (SKYBOX))
				l_selectedMat = GetMaterialManager()->GetSkyCubeMaterial("", "");
			else 
				l_selectedMat = GetMaterialManager()->GetGeometryPassMaterial("", "");
			
			a_renderable->SetMaterial(l_selectedMat);
			m_toRender[l_selectedMat].push_back(a_renderable);
			return true;
		} 
		return false;
	}

	void ADeferredShadingPass::RemoveRenderable(IRenderable * a_renderable)
	{
		typedef std::unordered_map< std::shared_ptr<IShaderProgram>, std::vector<IRenderable*> >::iterator Iter;
		for(Iter l_iter = m_toRender.begin(); l_iter != m_toRender.end(); ++l_iter )
		{
			std::vector<IRenderable*>::iterator l_iter2 = std::find(l_iter->second.begin(), l_iter->second.end(), a_renderable);
			if (l_iter2 != l_iter->second.end())
				l_iter->second.erase(l_iter2);
		}
	}

	bool ADeferredShadingPass::Exists(IRenderable * a_renderable) const
	{
		typedef std::unordered_map< std::shared_ptr<IShaderProgram>, std::vector<IRenderable*> >::const_iterator Iter;
		for (Iter l_iter = m_toRender.cbegin(); l_iter != m_toRender.cend(); ++l_iter)
		{
			std::vector<IRenderable*>::const_iterator l_iter2 = std::find(l_iter->second.cbegin(), l_iter->second.cend(), a_renderable);
			if (l_iter2 != l_iter->second.end())
				return true;
		}
		return false;
	}

	void ADeferredShadingPass::SetMaterialManager(MaterialControl::IMaterialManager * a_materialManager)
	{
		IRenderPass::SetMaterialManager(a_materialManager);

		// initiate materials
		std::shared_ptr<IShaderProgram> GeometryMaterial =
			a_materialManager->GetGeometryPassMaterial(
				"resources\\shaders\\GeometryShader.vert",
				"resources\\shaders\\GeometryShader.frag"
			);

		std::shared_ptr<IShaderProgram> GeometryMaterialEmissiveMap =
			GetMaterialManager()->GetEmissiveGeometryPassMaterial(
				"resources\\shaders\\GeometryShader.vert",
				"resources\\shaders\\GeometryEmissiveShader.frag"
			);

		std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseMap =
			a_materialManager->GetColourGeometryPassMaterial(
				"resources\\shaders\\GeometryShader.vert",
				"resources\\shaders\\GeometryColourShader.frag"
			);

		std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseNormalMap =
			a_materialManager->GetColourNormalGeometryPassMaterial(
				"resources\\shaders\\GeometryShader.vert",
				"resources\\shaders\\GeometryColourNormalShader.frag"
			);

		std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseNormalSpecMap =
			a_materialManager->GetColourNormalSpecGeometryPassMaterial(
				"resources\\shaders\\GeometryShader.vert",
				"resources\\shaders\\GeometryColourNormalSpecShader.frag"
			);

		std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseNormalSpecHardnessMap =
			a_materialManager->GetColourNormalSpecHardnessGeometryPassMaterial(
				"resources\\shaders\\GeometryShader.vert",
				"resources\\shaders\\GeometryColourNormalSpecHardnessShader.frag"
			);

		std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseNormalSpecHardnessEmissionMap =
			a_materialManager->GetColourNormalSpecHardnessEmissionGeometryPassMaterial(
				"resources\\shaders\\GeometryShader.vert",
				"resources\\shaders\\GeometryColourNormalSpecHardnessEmissiveShader.frag"
			);
		

		std::shared_ptr<IShaderProgram> GeometrySkybox =
			a_materialManager->GetSkyCubeMaterial(
				"resources\\shaders\\GeometrySkyBoxShader.vert",
				"resources\\shaders\\GeometrySkyBoxShader.frag"
			);

		// create appropriate keys
		m_toRender[GeometryMaterialDiffuseNormalSpecHardnessEmissionMap];
		m_toRender[GeometryMaterialDiffuseNormalSpecHardnessMap];
		m_toRender[GeometryMaterialDiffuseNormalSpecMap];
		m_toRender[GeometryMaterialDiffuseNormalMap];
		m_toRender[GeometryMaterialDiffuseMap];
		m_toRender[GeometryMaterialEmissiveMap];
		m_toRender[GeometryMaterial];
		m_toRender[GeometrySkybox];


		std::shared_ptr<IShaderProgram> l_nullMaterial =
			a_materialManager->GetStencilLightPassMaterial(
				"resources\\shaders\\NullShader.vert",
				"resources\\shaders\\NullShader.frag"
			);

		std::shared_ptr<IShaderProgram> l_pointLightMaterial =
			a_materialManager->GetPointLightPassMaterial(
				"resources\\shaders\\PointLightShader.vert",
				"resources\\shaders\\PointLightShader.frag"
			);

		std::shared_ptr<IShaderProgram> l_spotLightMaterial =
			a_materialManager->GetSpotLightPassMaterial(
				"resources\\shaders\\SpotLightShader.vert",
				"resources\\shaders\\SpotLightShader.frag"
			);

		std::shared_ptr<IShaderProgram> l_directionalLightMaterial =
			a_materialManager->GetFullScreenLightPassMaterial(
				"resources\\shaders\\DirectionalLightShader.vert",
				"resources\\shaders\\DirectionalLightShader.frag"
			);


	}

	void ADeferredShadingPass::AddLight(IRenderable * a_light, const LightTypeFlags& a_lightType)
	{
		switch (a_lightType)
		{
		case POINT_LIGHT:
			a_light->SetMaterial( m_materialManager->GetStencilLightPassMaterial("",""), 1);
			a_light->SetMaterial( m_materialManager->GetPointLightPassMaterial("",""), 0);
			break;
		case SPOT_LIGHT:
			a_light->SetMaterial(m_materialManager->GetStencilLightPassMaterial("", ""), 1);
			a_light->SetMaterial(m_materialManager->GetSpotLightPassMaterial("", ""), 0);
			break;
		case DIRECTIONAL_LIGHT:
			a_light->SetMaterial(m_materialManager->GetFullScreenLightPassMaterial("", "") );
			break;
		}

		m_lights.insert(a_light);
	}



}