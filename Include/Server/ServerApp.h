#pragma once


#include "Server/ServerControl.h"
#include "Common/Core/Gamewindow.h"
#include "Common/Core/HighResolutionTimer.h"
#include "Common/Core/MyUtilities.h"
#include "Common/AGraphicsEngine.h"
#include <windows.h>

#include "Common/RenderControl/Camera.h"
#include "Common/SceneControl/CameraSceneNode.h"

class ServerApp
{
public:
  ServerApp(const glm::vec2& a_dimensions, const ImplTech& a_implTech = ImplTech::OPENGL, const unsigned int& a_clientsCount = 1);
  ~ServerApp();
  
  
  LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);  
  void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();
  
  
private:
  HINSTANCE m_hInstance;
  GameWindow m_gameWindow;
  CHighResolutionTimer *m_pHighResolutionTimer;
  double m_dt;  ///< last frame time
  double m_elapsedTime;   ///< used to count frames per second
  unsigned int m_frameCount;
  bool m_appActive;
  glm::vec2 m_dimensions;
  
  Network::ServerControl m_serverCtrl;
  std::map<std::shared_ptr<asio::ip::tcp::socket>, unsigned int> m_clients;
  AGraphicsEngine* m_graphics;
  ImplTech m_implTech;
  unsigned int m_clientsCount;
  unsigned int m_clientsToCompleteFrame;
  
  
  // a preallocated data array for the raw decoded texture that is used to update the composition rectangles
  char* m_textureData;
  
  SceneControl::CameraSceneNode* m_camera; /// use this camera in order to get updated movement values to send to the clients
  
  void Update();
  void UpdateScene();
  void Initialise();
  void InitialiseScene();

};