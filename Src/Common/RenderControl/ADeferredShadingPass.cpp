#include "Common/RenderControl/ADeferredShadingPass.h"
#include <memory>


namespace RenderControl
{
	ADeferredShadingPass::ADeferredShadingPass(const glm::vec2& a_resolution, const glm::vec2 &a_partialResolution, const glm::vec4& a_viewportSettings)
		:IRenderPass(a_resolution), m_resolutionPart(a_partialResolution), m_viewPortSetting(a_viewportSettings)
    {
      GetCamera()->SetDimentsions(a_resolution);
    }


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

	}

	void ADeferredShadingPass::AddLight(IRenderable * a_light, const LightTypeFlags& a_lightType)
	{
		switch (a_lightType)
		{
		case LightTypeFlags::POINT_LIGHT:
			a_light->SetMaterial( m_materialManager->GetStencilLightPassMaterial("",""), 1);
			a_light->SetMaterial( m_materialManager->GetPointLightPassMaterial("",""), 0);
			break;
		case LightTypeFlags::SPOT_LIGHT:
			a_light->SetMaterial(m_materialManager->GetStencilLightPassMaterial("", ""), 1);
			a_light->SetMaterial(m_materialManager->GetSpotLightPassMaterial("", ""), 0);
			break;
		case LightTypeFlags::DIRECTIONAL_LIGHT:
			a_light->SetMaterial(m_materialManager->GetFullScreenLightPassMaterial("", "") );
			break;
		}

		m_lights.insert(a_light);
	}



}