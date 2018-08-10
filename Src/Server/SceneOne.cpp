#include "Server/SceneOne.h"


SceneOne::SceneOne(Network::ServerControl& a_serverCtrl, AGraphicsEngine* a_graphics, const std::vector<std::shared_ptr<asio::ip::tcp::socket> >& a_clients, SceneControl::CameraSceneNode* a_camera)
  : IServerSceneController(a_serverCtrl, a_graphics, a_clients, a_camera)
{
  
}

void SceneOne::Init()
{
  unsigned int  m_nextID = 1;


  Network::ObjAddInfo l_dragon;
  l_dragon.m_id = m_nextID;
  l_dragon.m_objType = Network::ObjectType::MESH;
  l_dragon.m_materialFlags = RenderControl::GeometryPassMaterialFlags::SIMPLE_GEOMETRY;
  l_dragon.m_meshPath = std::string("../Assets/Models/stanford_dragon/dragon.obj");
  m_objectsToAdd.push_back(l_dragon);
  
  
  Network::ObjTransformInfo l_dragTrans;
  l_dragTrans.m_id = m_nextID;
  l_dragTrans.m_transformType = Network::ObjectTransformType::OBJ_POS;
  l_dragTrans.x = 0;
  l_dragTrans.y = 0;
  l_dragTrans.z = -10;
  m_objectsToTransform.push_back(l_dragTrans);
  
  ++m_nextID;
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
  
  // point lights
  unsigned int l_pointLightCount = 120;
  for( int i = 0; i < l_pointLightCount; ++i)
  {
    ++m_nextID;
    
    l_light.m_id = m_nextID;
    l_light.m_lightFlags = RenderControl::LightTypeFlags::POINT_LIGHT;
    m_lightsToAdd.push_back(l_light);
    
    
    l_lightTransform.m_id = m_nextID;
    l_lightTransform.m_transformType = Network::ObjectTransformType::LGHT_DIFFUSE;
    l_lightTransform.x = (double)rand() / RAND_MAX;
    l_lightTransform.y = (double)rand() / RAND_MAX;
    l_lightTransform.z = (double)rand() / RAND_MAX;
    m_lightsToTransform.push_back(l_lightTransform);
    
    
    l_lightTransform.m_id = m_nextID;
    l_lightTransform.m_transformType = Network::ObjectTransformType::OBJ_POS;
    l_lightTransform.x = rand() % 30 - 15;
    l_lightTransform.y = rand() % 8 + 1;
    l_lightTransform.z = -(rand() % 20);
    m_lightsToTransform.push_back(l_lightTransform);
    m_lightsToTransformPos.push_back(l_lightTransform);
    m_lightTargets.push_back( glm::vec3(0) );
    m_lightSourceSpeeds.push_back( glm::vec3(0) );
  }
  
  
  Network::NetworkMsgPtr l_msg = std::make_shared<Network::NetworkMsg>();
  l_msg->CreateSceneUpdateMsg(
  {glm::vec3(0,0,0), glm::vec3(0,0,-1), glm::vec3(0,1,0) },
   m_objectsToAdd, {}, m_objectsToTransform, {}, 
   m_lightsToAdd, {}, m_lightsToTransform );

  

  for( std::vector<std::shared_ptr<asio::ip::tcp::socket> >::iterator l_iter = m_clients.begin(); l_iter != m_clients.end(); ++l_iter )
    m_serverCtrl.PushMsg(*l_iter, l_msg);  
}

void SceneOne::Update(double a_dt)
{
  Network::NetworkMsgPtr l_msg = std::make_shared<Network::NetworkMsg>();

  
  
  LightPosStep();

  l_msg->CreateSceneUpdateMsg(
  {m_camera->GetCamera()->GetPosition(), m_camera->GetCamera()->GetView(), m_camera->GetCamera()->GetUpVector() },
  {}, {}, {}, {}, {}, {}, m_lightsToTransformPos);

  
  for( std::vector<std::shared_ptr<asio::ip::tcp::socket> >::iterator l_iter = m_clients.begin(); l_iter != m_clients.end(); ++l_iter )
    m_serverCtrl.PushMsg(*l_iter, l_msg);
}


void SceneOne::LightPosStep()
{
  float l_speedMag = 1.f;
  for( unsigned int i = 0; i < m_lightsToTransformPos.size(); ++i)
  {
    glm::vec3 l_currPos( m_lightsToTransformPos[i].x, m_lightsToTransformPos[i].y, m_lightsToTransformPos[i].z);
    glm::vec3 l_targetPos = m_lightTargets[i];
    glm::vec3 l_currDir = l_targetPos - l_currPos;
    
    
    while( glm::length( l_currDir ) < l_speedMag )
    {
      l_targetPos.x = rand() % 30 - 15;
      l_targetPos.y = rand() % 8 + 1;
      l_targetPos.z = -(rand() % 20);
      
      l_currDir = l_targetPos - l_currPos;
    }
    
    m_lightTargets[i] = l_targetPos;
    
    l_currDir = l_speedMag * glm::normalize ( l_currDir  );
    
    m_lightsToTransformPos[i].x += l_currDir.x;
    m_lightsToTransformPos[i].y += l_currDir.y;
    m_lightsToTransformPos[i].z += l_currDir.z;

  }
}

