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

  // vectors to hold scene update data
  std::vector<Network::ObjAddInfo> m_outObjsToAdd; 
  std::vector<uint32_t> m_outObjsToRemove;
  std::vector<Network::ObjTransformInfo> m_outObjsToTransform; 
  std::vector<Network::TextureChangeInfo> m_outTextureChange;
  std::vector<Network::ObjAddInfo> m_outLightsToAdd; 
  std::vector<uint32_t> m_outLightsToRemove;
  std::vector<Network::ObjTransformInfo> m_outLightsToTransform;

  Network::NetworkMsgPtr m_renderResultMsg;
  
  
  void Initialise();
  void Update();
  
  // helpers
  void AddObject(const Network::ObjAddInfo& a_info);
  void RemoveObject(const uint32_t& a_id, bool a_isLight);
  void TransformObject(const Network::ObjTransformInfo& a_info, bool a_isLight);
  void TextureChange(const Network::TextureChangeInfo& a_info);
  void AddLight(const Network::ObjAddInfo& a_info);
  
public:
  ClientApp( const std::string &a_hostName, const unsigned int &a_hostPort, const ImplTech& a_implTech = ImplTech::OPENGL);
  ~ClientApp();
  
  LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
  WPARAM Execute();
  void SetHinstance(HINSTANCE hinstance);
  
  
};

