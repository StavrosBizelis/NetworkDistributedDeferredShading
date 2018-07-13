#pragma once

#include "Common/AGraphicsEngine.h"

class GLGraphicsEngine : public AGraphicsEngine
{
private:
  RenderControl::ADeferredShadingPass* m_deferredShadingPass;
  RenderControl::ACompositionPass* m_compositionPass;

  IShapeFactory* m_shapeFactory;
  ITextureFactory* m_textureFactory;
  
public:
  GLGraphicsEngine(const glm::vec2& a_resolution = glm::vec2(0));
  GLGraphicsEngine(const glm::vec2& a_resolution, const glm::vec2 &a_partialResolution, const glm::vec4& a_viewportSettings);
  virtual ~GLGraphicsEngine();
  
  virtual void Init(bool a_composite = false, unsigned int a_subparts = 0);

  virtual RenderControl::ADeferredShadingPass* GetDeferredRenderPass()  const;
  virtual RenderControl::ACompositionPass* GetCompositionPass() const;
  
  virtual IShapeFactory* GetShapeFactory()  const;
  virtual ITextureFactory* GetTextureFactory() const;
};