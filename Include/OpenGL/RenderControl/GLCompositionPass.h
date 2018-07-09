#pragma once
#include "Common/RenderControl/ACompositionPass.h"
#include "Common/MaterialControl/IShaderProgram.h"

namespace RenderControl
{
  
  
  
	/// A renderpass that implements deferred shading
	class GLCompositionPass :
		public ACompositionPass
	{
  protected:
    std::shared_ptr<IShaderProgram> m_material;
    
    unsigned int m_fbo;    
    void Clear();
    
	public:
		GLCompositionPass(const glm::vec2& a_resolution, SceneControl::SceneManager* a_scnManager, IShapeFactory* a_shapeFactory, ITextureFactory* a_textFactory, const unsigned int& a_subparts);
		virtual ~GLCompositionPass();
    
    
    inline virtual void SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager);
		
    
    
		virtual bool Init() override;
		virtual void Render() override;
		virtual void OutputOnScreen() override;
		virtual unsigned int GetFBO() override { return m_fbo; }

    
	};

}