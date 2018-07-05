#pragma once

#include "Client/ClientControl.h"
#include "Common/AGraphicsEngine.h"
#include "Common/Core/MyUtilities.h"

#include "Common/Core/Gamewindow.h"
#include "Common/Core/HighResolutionTimer.h"

class ClientApp
{
private:
  HINSTANCE m_hInstance;
  GameWindow m_gameWindow;
  
  CHighResolutionTimer *m_pHighResolutionTimer;
  double m_dt;
  double m_elapsedTime;   ///< used to count frames per second
  unsigned int m_frameCount;
  bool m_appActive;
  glm::vec2 m_dimensions;
  
  ImplTech m_implTech;
  Network::ClientControl m_client;
  AGraphicsEngine* m_graphics;

  // vectors to hold scene update data
  std::array<glm::vec3, 3> m_outCameraSettings;
  std::vector<Network::ObjAddInfo> m_outObjsToAdd; 
  std::vector<uint32_t> m_outObjsToRemove;
  std::vector<Network::ObjTransformInfo> m_outObjsToTransform; 
  std::vector<Network::TextureChangeInfo> m_outTextureChange;
  std::vector<Network::ObjAddInfo> m_outLightsToAdd; 
  std::vector<uint32_t> m_outLightsToRemove;
  std::vector<Network::ObjTransformInfo> m_outLightsToTransform;

  bool m_hasUpdated[3];
  
  void Initialise();
  void Update();
  
  SceneControl::CameraSceneNode* m_camera;
  // helpers
  void SetCamera(glm::vec3 a_pos, glm::vec3 a_view, glm::vec3 a_up);
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

