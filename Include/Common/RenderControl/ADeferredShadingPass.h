#pragma once
#include "Common/RenderControl/IRenderPass.h"

#include "Common/MaterialControl/IMaterialManager.h"
#include <unordered_map>
#include <set>
namespace RenderControl
{
	enum GeometryPassMaterialFlags
	{
		SIMPLE_GEOMETRY = 0,
		NORMAL_MAP = 1 << 0,
		DIFFUSE_MAP = 1 << 1,
		SPECULAR_MAP = 1 << 2,
		HARDNESS_MAP = 1 << 3,
		EMISSION_MAP = 1 << 4,
		SKYBOX = 1 << 5
	};

	enum class LightTypeFlags : char
	{
		POINT_LIGHT = 0,
		SPOT_LIGHT,
		DIRECTIONAL_LIGHT
	};

	/// A renderpass that implements deferred shading
	class ADeferredShadingPass :
		public IRenderPass
	{
  protected:
		std::set<IRenderable*> m_lights;	///< does not own these lights
											
		/// Render pass does not own IRenderable objects
		std::unordered_map< std::shared_ptr<IShaderProgram>, std::vector<IRenderable*> > m_toRender;

    glm::vec2 m_resolutionPart;
    glm::vec4 m_viewPortSetting;

	public:
		ADeferredShadingPass(const glm::vec2& a_resolution, const glm::vec2 &a_partialResolution, const glm::vec4& a_viewportSettings);
		virtual ~ADeferredShadingPass();

    
		virtual bool AddRenderable(IRenderable* a_renderable, const GeometryPassMaterialFlags& a_geometryMaterialFlags = GeometryPassMaterialFlags::SIMPLE_GEOMETRY );
		
		inline void RemoveRenderable(IRenderable* a_renderable);
		inline bool Exists(IRenderable* a_renderable) const;

		virtual void SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager);
		/// in a deferred shading technique lights are renderable meshes
		/// however the material and mesh must be set before adding it to the renderer
		void AddLight(IRenderable* a_light, const LightTypeFlags& a_lightType);
    virtual void UpdateViewportSettings(const glm::vec2& a_resolution, const glm::vec4& a_viewportSettings) = 0;
		// virtual bool Init() override;
		// virtual void Render() override;
		// virtual void OutputOnScreen() override;
		// virtual unsigned int GetFBO() override { return m_fbo; }

	};

}