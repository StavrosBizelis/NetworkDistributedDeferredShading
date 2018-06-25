#pragma once

#include "Common/Shapes/IMesh.h"
#include "Common/Shapes/IShapeFactory.h"
#include <memory>
#include <map>

class GLShapeFactory : public IShapeFactory
{
  std::shared_ptr<ACone> m_cone;
  std::shared_ptr<ACube> m_cube;
  std::map<std::string, std::shared_ptr<AOpenAssetImportMesh> > m_meshes;
  std::shared_ptr<ARect> m_rect;
  std::shared_ptr<ASkybox> m_skybox;
  std::shared_ptr<ASphere> m_sphere;
  
  
public:
  virtual void Init();

  virtual std::shared_ptr<ACone> GetCone();
  virtual std::shared_ptr<ACube> GetCube();
  virtual std::shared_ptr<AOpenAssetImportMesh> GetOpenAssetImportMesh(const std::string& a_filename);
  virtual std::shared_ptr<ARect> GetRectangle();
  virtual std::shared_ptr<ASkybox> GetSkybox();
  virtual std::shared_ptr<ASphere> GetSphere();
};