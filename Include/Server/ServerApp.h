#pragma once


#include "Server/ServerControl.h"
#include "Server/Gamewindow.h"
#include "Common/Core/HighResolutionTimer.h"
#include "Common/Core/MyUtilities.h"
#include "Common/AGraphicsEngine.h"
#include <windows.h>


class ServerApp
{
public:
  ServerApp(const glm::vec2& a_dimensions, const ImplTech& a_implTech = ImplTech::OPENGL);
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
  
  Network::ServerControl m_serverCtrl;
  std::vector<std::shared_ptr<asio::ip::tcp::socket> > m_sockets;
  AGraphicsEngine* m_graphics;
  ImplTech m_implTech;

  
  void Update();
  void Initialise();

};