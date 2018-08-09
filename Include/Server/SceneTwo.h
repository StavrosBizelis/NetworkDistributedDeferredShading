#pragma once

#include "Server/IServerSceneController.h"


class SceneTwo : public IServerSceneController
{
  protected:
  std::vector<Network::ObjAddInfo> m_objectsToAdd;
  
  std::vector<Network::ObjTransformInfo> m_objectsToTransform;
  std::vector<Network::ObjTransformInfo> m_torusesToRotate;
  std::vector<Network::ObjTransformInfo> m_spheresToMove;
  std::vector<Network::ObjTransformInfo> m_spheresToRotate;
  std::vector<Network::TextureChangeInfo> m_objectsTextures;
  
  std::vector<Network::ObjAddInfo> m_lightsToAdd;
  std::vector<Network::ObjTransformInfo> m_lightsToTransformPos;
  std::vector<glm::vec3> m_lightTargets;
  std::vector<Network::ObjTransformInfo> m_lightsToTransform;
  
  
  std::vector<glm::vec3> m_lightSourceSpeeds;
  float m_elapsedTime;
  
  void LightMoveHelper();
  public:
  SceneTwo(Network::ServerControl& a_serverCtrl, AGraphicsEngine* a_graphics, const std::vector<std::shared_ptr<asio::ip::tcp::socket> >& a_clients, SceneControl::CameraSceneNode* a_camera);
  
  virtual void Init();
  virtual void Update(double a_dt);
};