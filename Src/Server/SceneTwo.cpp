#include "Server/SceneTwo.h"


SceneTwo::SceneTwo(Network::ServerControl& a_serverCtrl, AGraphicsEngine* a_graphics, const std::vector<std::shared_ptr<asio::ip::tcp::socket> >& a_clients, SceneControl::CameraSceneNode* a_camera)
  : IServerSceneController(a_serverCtrl, a_graphics, a_clients, a_camera)
{
  
}

void SceneTwo::Init()
{
  unsigned int  m_nextID = 1;

  
  Network::ObjAddInfo l_baseCube;
  l_baseCube.m_id = m_nextID;
  l_baseCube.m_objType = Network::ObjectType::CUBE;
  l_baseCube.m_materialFlags = RenderControl::GeometryPassMaterialFlags::SIMPLE_GEOMETRY;
  m_objectsToAdd.push_back(l_baseCube);
  
  
  Network::ObjTransformInfo l_cubeTrans;
  l_cubeTrans.m_id = m_nextID;
  l_cubeTrans.m_transformType = Network::ObjectTransformType::OBJ_POS;
  l_cubeTrans.x = 0;
  l_cubeTrans.y = -10;
  l_cubeTrans.z = 0;
  m_objectsToTransform.push_back(l_cubeTrans);
  
  
  l_cubeTrans.m_id = m_nextID;
  l_cubeTrans.m_transformType = Network::ObjectTransformType::OBJ_SCALE;
  l_cubeTrans.x = 1000;
  l_cubeTrans.y = 20;
  l_cubeTrans.z = 1000;
  m_objectsToTransform.push_back(l_cubeTrans);
  
  
  
  ++m_nextID; // 3
  // directional light
  Network::ObjAddInfo l_light;
  l_light.m_id = m_nextID;
  l_light.m_lightFlags = RenderControl::LightTypeFlags::DIRECTIONAL_LIGHT;
  m_lightsToAdd.push_back(l_light);
  
  
  Network::ObjTransformInfo l_lightTransform;
  l_lightTransform.m_id = m_nextID;
  l_lightTransform.m_transformType = Network::ObjectTransformType::LGHT_DIFFUSE;
  l_lightTransform.x = 0.1;
  l_lightTransform.y = 0.1;
  l_lightTransform.z = 0.1;
  m_lightsToTransform.push_back(l_lightTransform);
  
  l_lightTransform.m_id = m_nextID;
  l_lightTransform.m_transformType = Network::ObjectTransformType::LGHT_AMBIENT;
  l_lightTransform.x = 0.02;
  l_lightTransform.y = 0.02;
  l_lightTransform.z = 0.02;
  m_lightsToTransform.push_back(l_lightTransform);
  
  l_lightTransform.m_id = m_nextID;
  l_lightTransform.m_transformType = Network::ObjectTransformType::OBJ_ROT;
  l_lightTransform.x = 30;
  l_lightTransform.y = 0;
  l_lightTransform.z = 0;
  m_lightsToTransform.push_back(l_lightTransform);
  
  
  
  Network::NetworkMsgPtr l_msg = std::make_shared<Network::NetworkMsg>();
  l_msg->CreateSceneUpdateMsg(
  {glm::vec3(0,0,0), glm::vec3(0,0,-1), glm::vec3(0,1,0) },
   m_objectsToAdd, {}, m_objectsToTransform, {}, 
   m_lightsToAdd, {}, m_lightsToTransform );

  

  for( std::vector<std::shared_ptr<asio::ip::tcp::socket> >::iterator l_iter = m_clients.begin(); l_iter != m_clients.end(); ++l_iter )
    m_serverCtrl.PushMsg(*l_iter, l_msg);  
}

void SceneTwo::Update(double a_dt)
{
  Network::NetworkMsgPtr l_msg = std::make_shared<Network::NetworkMsg>();

  l_msg->CreateSceneUpdateMsg(
  {m_camera->GetCamera()->GetPosition(), m_camera->GetCamera()->GetView(), m_camera->GetCamera()->GetUpVector() },
  {}, {}, {}, {}, {}, {}, {});

  
  for( std::vector<std::shared_ptr<asio::ip::tcp::socket> >::iterator l_iter = m_clients.begin(); l_iter != m_clients.end(); ++l_iter )
    m_serverCtrl.PushMsg(*l_iter, l_msg);
}
