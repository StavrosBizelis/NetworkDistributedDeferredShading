#pragma once


#include "Server/ServerControl.h"
#include "Common/Core/Gamewindow.h"
#include "Common/Core/HighResolutionTimer.h"
#include "Common/Core/MyUtilities.h"
#include "Common/AGraphicsEngine.h"
#include <windows.h>

#include "Common/RenderControl/Camera.h"
#include "Common/SceneControl/CameraSceneNode.h"

#include "Server/IServerSceneController.h"
#include <sstream>
class ServerApp
{
public:
  ServerApp(const glm::vec2& a_dimensions, const unsigned int& a_port, const ImplTech& a_implTech = ImplTech::OPENGL, const unsigned int& a_clientsCount = 1, const unsigned int& a_testIndex = 0, 
            const std::string& a_outFile = "", const unsigned int& a_numberOfLights = 10);
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
  double m_elapsedTime2;   ///< used to consistently update the clients
  unsigned int m_frameCount;
  bool m_appActive;
  glm::vec2 m_dimensions;
  
  Network::ServerControl m_serverCtrl;
  std::map<std::shared_ptr<asio::ip::tcp::socket>, unsigned int> m_clients;
  AGraphicsEngine* m_graphics;
  ImplTech m_implTech;
  unsigned int m_clientsCount;
  unsigned int m_testIndex;
  std::set<std::shared_ptr<asio::ip::tcp::socket> > m_sockets;
  std::set<std::shared_ptr<asio::ip::tcp::socket> > m_socketsToCompleteFrame;
  
  
  // a preallocated data array for the raw decoded texture that is used to update the composition rectangles
  char* m_textureData;
  
  SceneControl::CameraSceneNode* m_camera; /// use this camera in order to get updated movement values to send to the clients
  
  
  std::shared_ptr<IServerSceneController> m_sceneController;
  
  void Update();
  void UpdateScene();
  void Initialise();

  std::ostringstream m_output;
  std::string m_outFile;
  unsigned int m_numberOfLights;
};