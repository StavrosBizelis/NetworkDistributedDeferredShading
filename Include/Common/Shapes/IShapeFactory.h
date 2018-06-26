#pragma once

#include "Common/Shapes/IMesh.h"
#include <memory>

#include "Common/Shapes/ACone.h"
#include "Common/Shapes/ACube.h"
#include "Common/Shapes/AOpenAssetImportMesh.h"
#include "Common/Shapes/ARect.h"
#include "Common/Shapes/ASkybox.h"
#include "Common/Shapes/ASphere.h"

class IShapeFactory
{
public:
  virtual void Init() = 0;
  virtual std::shared_ptr<ACone> GetCone() const = 0;
  virtual std::shared_ptr<ACube> GetCube() const = 0;
  virtual std::shared_ptr<AOpenAssetImportMesh> GetOpenAssetImportMesh(const std::string& a_filename) = 0;
  virtual std::shared_ptr<ARect> GetRectangle() const = 0;
  virtual std::shared_ptr<ASkybox> GetSkybox() const = 0;
  virtual std::shared_ptr<ASphere> GetSphere() const = 0;
};