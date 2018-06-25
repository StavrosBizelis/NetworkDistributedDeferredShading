#pragma once

#include "Client/ClientControl.h"
#include "Common/AGraphicsEngine.h"
#include "Common/Core/MyUtilities.h"


class ClientApp
{
private:
  ImplTech m_implTech;
  Network::ClientControl* m_client;
  AGraphicsEngine* m_graphics;

  
public:
  ClientApp( const glm::vec2& a_dimensions, const ImplTech& a_implTech = ImplTech::OPENGL);
  ~ClientApp();
  
  void Initialise();
  void Update();
  
  
};

