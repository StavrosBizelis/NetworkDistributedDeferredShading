#pragma once

#include "Server/ServerControl.h"
#include "Common/Core/HighResolutionTimer.h"
#include "Common/Core/MyUtilities.h"
#include "Common/AGraphicsEngine.h"

#include "Common/RenderControl/Camera.h"
#include "Common/SceneControl/CameraSceneNode.h"

class IServerSceneController
{
  protected:
  Network::ServerControl& m_serverCtrl;
  AGraphicsEngine* m_graphics;
  std::vector<std::shared_ptr<asio::ip::tcp::socket> > m_clients;
  SceneControl::CameraSceneNode* m_camera; /// use this camera in order to get updated movement values to send to the clients
  
  public:
  IServerSceneController(Network::ServerControl& a_serverCtrl, AGraphicsEngine* a_graphics, const std::vector<std::shared_ptr<asio::ip::tcp::socket> >& a_clients, SceneControl::CameraSceneNode* a_camera)
  : m_serverCtrl(a_serverCtrl), m_graphics(a_graphics), m_clients(a_clients), m_camera(a_camera) {}
  
  virtual void Init() = 0;
  virtual void Update(double a_dt) = 0;
};