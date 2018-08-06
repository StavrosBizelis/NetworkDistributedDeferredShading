#pragma once

#include "Server/IServerSceneController.h"


class SceneTwo : public IServerSceneController
{
  protected:
  std::vector<Network::ObjAddInfo> m_objectsToAdd;
  std::vector<Network::ObjTransformInfo> m_objectsToTransform;
  
  std::vector<Network::ObjAddInfo> m_lightsToAdd;
  std::vector<Network::ObjTransformInfo> m_lightsToTransformPos;
  std::vector<Network::ObjTransformInfo> m_lightsToTransform;
  
  
  std::vector<glm::vec3> m_lightSourceSpeeds;
  
  public:
  SceneTwo(Network::ServerControl& a_serverCtrl, AGraphicsEngine* a_graphics, const std::vector<std::shared_ptr<asio::ip::tcp::socket> >& a_clients, SceneControl::CameraSceneNode* a_camera);
  
  virtual void Init();
  virtual void Update(double a_dt);
};