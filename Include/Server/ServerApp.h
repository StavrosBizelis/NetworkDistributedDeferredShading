#pragma once


#include "Server/ServerControl.h"
#include "Server/Gamewindow.h"
#include "Common/Core/HighResolutionTimer.h"

#include <windows.h>


class ServerApp
{
public:
  ServerApp();
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
  
  
  void Update();
  void Initialise();

};