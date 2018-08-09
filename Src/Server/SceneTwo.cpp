#include "Server/SceneTwo.h"


SceneTwo::SceneTwo(Network::ServerControl& a_serverCtrl, AGraphicsEngine* a_graphics, const std::vector<std::shared_ptr<asio::ip::tcp::socket> >& a_clients, SceneControl::CameraSceneNode* a_camera)
  : IServerSceneController(a_serverCtrl, a_graphics, a_clients, a_camera)
{
  
}

void SceneTwo::Init()
{
  unsigned int m_nextID = 1;

  
  Network::ObjAddInfo l_baseCube;
  l_baseCube.m_id = m_nextID;
  l_baseCube.m_objType = Network::ObjectType::CUBE;
  l_baseCube.m_materialFlags = RenderControl::GeometryPassMaterialFlags::SIMPLE_GEOMETRY;
  m_objectsToAdd.push_back(l_baseCube);
  
  
  Network::ObjTransformInfo l_cubeTrans;
  l_cubeTrans.m_id = m_nextID;
  l_cubeTrans.m_transformType = Network::ObjectTransformType::OBJ_POS;
  l_cubeTrans.x = 250;
  l_cubeTrans.y = -30;
  l_cubeTrans.z = 250;
  m_objectsToTransform.push_back(l_cubeTrans);
  
  
  l_cubeTrans.m_id = m_nextID;
  l_cubeTrans.m_transformType = Network::ObjectTransformType::OBJ_SCALE;
  l_cubeTrans.x = 1000;
  l_cubeTrans.y = 1;
  l_cubeTrans.z = 1000;
  m_objectsToTransform.push_back(l_cubeTrans);
  
  
  //  add toruses
  for( unsigned int i = 0; i < 10; ++i)
    for( unsigned int j = 0; j < 10; ++j)
    {
      ++m_nextID;
      Network::ObjAddInfo l_torus;
      l_torus.m_id = m_nextID;
      l_torus.m_objType = Network::ObjectType::MESH;
      l_torus.m_meshPath = std::string("../Assets/Models/Torus/CoridorTorus.obj");
      l_torus.m_materialFlags = RenderControl::GeometryPassMaterialFlags(RenderControl::GeometryPassMaterialFlags::DIFFUSE_MAP | RenderControl::GeometryPassMaterialFlags::NORMAL_MAP | RenderControl::GeometryPassMaterialFlags::SPECULAR_MAP);
      m_objectsToAdd.push_back(l_torus);
      
      
      Network::ObjTransformInfo l_torusTrans;
      l_torusTrans.m_id = m_nextID;
      l_torusTrans.m_transformType = Network::ObjectTransformType::OBJ_POS;
      l_torusTrans.x = i * 50;
      l_torusTrans.y = 0;
      l_torusTrans.z = j * 50;
      m_objectsToTransform.push_back(l_torusTrans);
      
      l_torusTrans.m_id = m_nextID;
      l_torusTrans.m_transformType = Network::ObjectTransformType::OBJ_ROT;
      l_torusTrans.x = 0;
      l_torusTrans.y = 0;
      l_torusTrans.z = 90;
      m_objectsToTransform.push_back(l_torusTrans);  
      m_torusesToRotate.push_back(l_torusTrans);
      
      
      l_torusTrans.m_id = m_nextID;
      l_torusTrans.m_transformType = Network::ObjectTransformType::OBJ_SCALE;
      l_torusTrans.x = 15;
      l_torusTrans.y = 15;
      l_torusTrans.z = 15;
      m_objectsToTransform.push_back(l_torusTrans);

      Network::TextureChangeInfo l_textures;
      l_textures.m_id = m_nextID;
      l_textures.m_textureLayer = 0;
      l_textures.m_cubeText = false;
      l_textures.m_path[0] = std::string("../Assets/Models/Torus/Diffuse.png");
      m_objectsTextures.push_back(l_textures);
      
      l_textures.m_id = m_nextID;
      l_textures.m_textureLayer = 1;
      l_textures.m_cubeText = false;
      l_textures.m_path[0] = std::string("../Assets/Models/Torus/Normal.png");
      m_objectsTextures.push_back(l_textures);
      
      l_textures.m_id = m_nextID;
      l_textures.m_textureLayer = 2;
      l_textures.m_cubeText = false;
      l_textures.m_path[0] = std::string("../Assets/Models/Torus/Specular.png");
      m_objectsTextures.push_back(l_textures);
    }
  
  for( unsigned int i = 0; i < 10; ++i)
    for( unsigned int j = 0; j < 10; ++j)
    {
      ++m_nextID;
      Network::ObjAddInfo l_sphere;
      l_sphere.m_id = m_nextID;
      l_sphere.m_objType = Network::ObjectType::MESH;
      l_sphere.m_meshPath = std::string("../Assets/Models/Sphere/sphere.obj");
      l_sphere.m_materialFlags = RenderControl::GeometryPassMaterialFlags(RenderControl::GeometryPassMaterialFlags::DIFFUSE_MAP | RenderControl::GeometryPassMaterialFlags::NORMAL_MAP );
      m_objectsToAdd.push_back(l_sphere);
      
      
      Network::ObjTransformInfo l_sphereTrans;
      l_sphereTrans.m_id = m_nextID;
      l_sphereTrans.m_transformType = Network::ObjectTransformType::OBJ_POS;
      l_sphereTrans.x = i * 50;
      l_sphereTrans.y = 0;
      l_sphereTrans.z = j * 50;
      m_objectsToTransform.push_back(l_sphereTrans);
      m_spheresToMove.push_back(l_sphereTrans);
      
      
      l_sphereTrans.m_id = m_nextID;
      l_sphereTrans.m_transformType = Network::ObjectTransformType::OBJ_ROT;
      l_sphereTrans.x = 0;
      l_sphereTrans.y = 0;
      l_sphereTrans.z = 0;
      m_objectsToTransform.push_back(l_sphereTrans);  
      m_spheresToRotate.push_back(l_sphereTrans);  
      
      
      l_sphereTrans.m_id = m_nextID;
      l_sphereTrans.m_transformType = Network::ObjectTransformType::OBJ_SCALE;
      l_sphereTrans.x = 7;
      l_sphereTrans.y = 7;
      l_sphereTrans.z = 7;
      m_objectsToTransform.push_back(l_sphereTrans);

      Network::TextureChangeInfo l_textures;
      l_textures.m_id = m_nextID;
      l_textures.m_textureLayer = 0;
      l_textures.m_cubeText = false;
      l_textures.m_path[0] = std::string("../Assets/Models/Sphere/Diffuse.png");
      m_objectsTextures.push_back(l_textures);
      
      l_textures.m_id = m_nextID;
      l_textures.m_textureLayer = 1;
      l_textures.m_cubeText = false;
      l_textures.m_path[0] = std::string("../Assets/Models/Sphere/Normal.png");
      m_objectsTextures.push_back(l_textures);
    }
  
  

  
  
  
  
  ++m_nextID; // 3
  //  directional light
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
  unsigned int l_pointLightCount = 400;
  for( int i = 0; i < l_pointLightCount; ++i)
  {
    ++m_nextID;
    
    l_light.m_id = m_nextID;
    l_light.m_lightFlags = RenderControl::LightTypeFlags::POINT_LIGHT;
    m_lightsToAdd.push_back(l_light);
    
    
    l_lightTransform.m_id = m_nextID;
    l_lightTransform.m_transformType = Network::ObjectTransformType::LGHT_DIFFUSE;
    l_lightTransform.x = (double)rand() / RAND_MAX * 2;
    l_lightTransform.y = (double)rand() / RAND_MAX * 2;
    l_lightTransform.z = (double)rand() / RAND_MAX * 2;
    m_lightsToTransform.push_back(l_lightTransform);
    
    l_lightTransform.m_id = m_nextID;
    l_lightTransform.m_transformType = Network::ObjectTransformType::LGHT_ATTENUATION;
    l_lightTransform.x = (double)0;
    l_lightTransform.y = (double)0.001;
    l_lightTransform.z = (double)0.04;
    m_lightsToTransform.push_back(l_lightTransform);
    
    
    l_lightTransform.m_id = m_nextID;
    l_lightTransform.m_transformType = Network::ObjectTransformType::OBJ_POS;
    l_lightTransform.x = rand() % 600 - 100;
    l_lightTransform.y = rand() % 40 - 20;
    l_lightTransform.z = rand() % 600 - 100;
    m_lightsToTransform.push_back(l_lightTransform);
    m_lightsToTransformPos.push_back(l_lightTransform);
    m_lightSourceSpeeds.push_back( glm::vec3(0) );
    m_lightTargets.push_back(glm::vec3( rand() % 500, rand() % 2+1, rand() % 500) );
    // l_offset of light in order not to all be at the center at the same time
    // unsigned int l_offset = rand() % 100;
    // for( unsigned int j = 0; j < l_offset; ++j)
      // LightPosStep(m_lightSourceSpeeds.size()-1, 10);
  }
  
  
  
  Network::NetworkMsgPtr l_msg = std::make_shared<Network::NetworkMsg>();
  l_msg->CreateSceneUpdateMsg(
  {glm::vec3(0,0,0), glm::vec3(0,0,-1), glm::vec3(0,1,0) },
   m_objectsToAdd, {}, m_objectsToTransform, m_objectsTextures, 
   m_lightsToAdd, {}, m_lightsToTransform );

  

  for( std::vector<std::shared_ptr<asio::ip::tcp::socket> >::iterator l_iter = m_clients.begin(); l_iter != m_clients.end(); ++l_iter )
    m_serverCtrl.PushMsg(*l_iter, l_msg);  
}

void SceneTwo::Update(double a_dt)
{
  m_elapsedTime += a_dt;
  if( 10*m_elapsedTime/1000 >= 360 )
    m_elapsedTime = 0;
  for( unsigned int i = 0; i < m_torusesToRotate.size(); ++i)
    m_torusesToRotate[i].y += ( (float)a_dt/1000.f ) * 10;
  
  for( unsigned int i = 0; i < m_spheresToRotate.size(); ++i)
    m_spheresToRotate[i].y -= ( (float)a_dt/1000.f ) * 10;

  
  float l_toRads = 3.14159265/180;
  float l_seconds = m_elapsedTime/10;
  for( unsigned int i = 0; i < m_spheresToMove.size(); ++i)
  {
    unsigned int l_offset = (i*360/9);
    m_spheresToMove[i].y = sin( (l_offset+l_seconds)*l_toRads) * 20;
  }
  
  std::vector<Network::ObjTransformInfo> l_toTrans = m_torusesToRotate;
  l_toTrans.insert(l_toTrans.end(), m_spheresToRotate.begin(), m_spheresToRotate.end() ); 
  l_toTrans.insert(l_toTrans.end(), m_spheresToMove.begin(), m_spheresToMove.end() );
  
  LightMoveHelper();
  
  Network::NetworkMsgPtr l_msg = std::make_shared<Network::NetworkMsg>();

  l_msg->CreateSceneUpdateMsg(
  {m_camera->GetCamera()->GetPosition(), m_camera->GetCamera()->GetView(), m_camera->GetCamera()->GetUpVector() },
  {}, {}, l_toTrans, {}, {}, {}, {m_lightsToTransformPos});

  
  for( std::vector<std::shared_ptr<asio::ip::tcp::socket> >::iterator l_iter = m_clients.begin(); l_iter != m_clients.end(); ++l_iter )
    m_serverCtrl.PushMsg(*l_iter, l_msg);
}


void SceneTwo::LightMoveHelper()
{
  float l_speedMag = 2.f;
  for( unsigned int i = 0; i < m_lightsToTransformPos.size(); ++i)
  {
    glm::vec3 l_currPos( m_lightsToTransformPos[i].x, m_lightsToTransformPos[i].y, m_lightsToTransformPos[i].z);
    glm::vec3 l_targetPos = m_lightTargets[i];
    glm::vec3 l_currDir = l_targetPos - l_currPos;
    
    
    while( glm::length( l_currDir ) < l_speedMag )
    {
      l_targetPos.x = rand() % 600 - 100;
      l_targetPos.y = rand() % 40 - 20;
      l_targetPos.z = rand() % 600 - 100;
      
      l_currDir = l_targetPos - l_currPos;
    }
    
    m_lightTargets[i] = l_targetPos;
    
    l_currDir = l_speedMag * glm::normalize ( l_currDir  );
    
    m_lightsToTransformPos[i].x += l_currDir.x;
    m_lightsToTransformPos[i].y += l_currDir.y;
    m_lightsToTransformPos[i].z += l_currDir.z;

  }
}