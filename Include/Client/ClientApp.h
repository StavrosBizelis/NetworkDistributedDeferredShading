#pragma once

#include "Client/ClientControl.h"
#include "Common/AGraphicsEngine.h"
#include "Common/Core/MyUtilities.h"

#include "Common/Core/Gamewindow.h"

class ClientApp
{
private:
  HINSTANCE m_hInstance;
  GameWindow m_gameWindow;
  
  ImplTech m_implTech;
  Network::ClientControl m_client;
  AGraphicsEngine* m_graphics;

  void Initialise();
  void Update();
  
public:
  ClientApp( const std::string &a_hostName, const unsigned int &a_hostPort, const ImplTech& a_implTech = ImplTech::OPENGL);
  ~ClientApp();
  
  WPARAM Execute();
  void SetHinstance(HINSTANCE hinstance);
  
  
};

