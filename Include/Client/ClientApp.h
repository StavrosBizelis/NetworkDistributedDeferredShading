#pragma once

#include "Client/ClientControl.h"





class ClientApp
{
private:
  Network::ClientControl m_client;
  
  RenderControl::RenderPassPipeline* m_renderMnger;
	SceneControl::SceneManager* m_sceneMnger;
  
public:
  ClientApp();
  ~ClientApp();
  
  void Initialise();
  void Update();
  
  
};

