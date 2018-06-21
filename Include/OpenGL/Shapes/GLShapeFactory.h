#pragma once

#include "Common/Shapes/IMesh.h"
#include "Common/Shapes/IShapeFactory.h"
#include <memory>
#include <map>

class GLShapeFactory : public IShapeFactory
{
  std::shared_ptr<IMesh> m_cone;
  std::shared_ptr<IMesh> m_cube;
  std::map<std::string, std::shared_ptr<IMesh> > m_meshes;
  std::shared_ptr<IMesh> m_rect;
  std::shared_ptr<IMesh> m_skybox;
  std::shared_ptr<IMesh> m_sphere;
  
  
public:
  virtual void Init();

  virtual std::shared_ptr<IMesh> GetCone();
  virtual std::shared_ptr<IMesh> GetCube();
  virtual std::shared_ptr<IMesh> GetOpenAssetImportMesh(const std::string& a_filename);
  virtual std::shared_ptr<IMesh> GetRectangle();
  virtual std::shared_ptr<IMesh> GetSkybox();
  virtual std::shared_ptr<IMesh> GetSphere();
};