#pragma once
#include "Common/RenderControl/ADeferredShadingPass.h"

#include "Common/MaterialControl/IMaterialManager.h"
#include <unordered_map>
namespace RenderControl
{

	/// A renderpass that implements deferred shading
	class GLDeferredShadingPass :
		public ADeferredShadingPass
	{
  protected:
    unsigned int m_fbo;

    glm::vec2 m_resolutionPart;
    glm::vec4 m_viewPortSetting;

    void GeometryPass();
    void LightPass();
    void Clear();
    
	public:
		GLDeferredShadingPass(const glm::vec2& a_resolution);
		virtual ~GLDeferredShadingPass();

		virtual bool Init() override;
		virtual void Render() override;
		virtual void OutputOnScreen() override;
		virtual unsigned int GetFBO() override { return m_fbo; }
    
    virtual void UpdateViewportSettings(const glm::vec2& a_resolution, const glm::vec4& a_viewportSettings);

    virtual void SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager);
	};

}