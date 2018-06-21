#pragma once

#include "Common/Shapes/IMesh.h"
#include <memory>

class IShapeFactory
{
public:
  virtual void Init() = 0;
  virtual std::shared_ptr<IMesh> GetCone() = 0;
  virtual std::shared_ptr<IMesh> GetCube() = 0;
  virtual std::shared_ptr<IMesh> GetOpenAssetImportMesh(const std::string& a_filename) = 0;
  virtual std::shared_ptr<IMesh> GetRectangle() = 0;
  virtual std::shared_ptr<IMesh> GetSkybox() = 0;
  virtual std::shared_ptr<IMesh> GetSphere() = 0;
};