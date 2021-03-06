#pragma once

#include "Server/IServerSceneController.h"


class SceneOne : public IServerSceneController
{
  protected:
  std::vector<Network::ObjAddInfo> m_objectsToAdd;
  std::vector<Network::ObjTransformInfo> m_objectsToTransform;
  
  std::vector<Network::ObjAddInfo> m_lightsToAdd;
  std::vector<Network::ObjTransformInfo> m_lightsToTransformPos;
  std::vector<Network::ObjTransformInfo> m_lightsToTransform;
  
  std::vector<glm::vec3> m_lightTargets;
  
  std::vector<glm::vec3> m_lightSourceSpeeds;
  
  void LightPosStep();
  
  public:
  SceneOne(Network::ServerControl& a_serverCtrl, AGraphicsEngine* a_graphics, const std::vector<std::shared_ptr<asio::ip::tcp::socket> >& a_clients, SceneControl::CameraSceneNode* a_camera, const unsigned int& a_numberOfLight);
  
  virtual void Init();
  virtual void Update(double a_dt);
};