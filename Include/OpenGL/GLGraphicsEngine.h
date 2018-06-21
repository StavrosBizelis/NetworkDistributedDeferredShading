#pragma once

#include "Common/AGraphicsEngine.h"

class GLGraphicsEngine : public AGraphicsEngine
{
private:
  RenderControl::ADeferredShadingPass* m_deferredShadingPass;

  IShapeFactory* m_shapeFactory;
  ITextureFactory* m_textureFactory;
  
public:
  GLGraphicsEngine(const glm::vec2& a_dimensions);
  virtual ~GLGraphicsEngine();
  
  virtual void Init();

  virtual RenderControl::ADeferredShadingPass* GetDeferredRenderPass()  const;
  
  virtual IShapeFactory* GetShapeFactory()  const;
  virtual ITextureFactory* GetTextureFactory() const;
};