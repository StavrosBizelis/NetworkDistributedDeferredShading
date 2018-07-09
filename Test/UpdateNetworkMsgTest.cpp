#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#include <asio-1.12.1/include/asio.hpp>
#include <windows.h>
#include "Server/ServerControl.h"
#include "Client/ClientControl.h"
#include "Common/Core/MyUtilities.h"
#include "Common/Network/NetworkMsg.h"

int main(int argc, const char * argv[] ) 
{
  Network::ObjAddInfo l_sky;
  l_sky.m_id = 1;
  l_sky.m_objType = Network::ObjectType::SKYBOX;
  l_sky.m_materialFlags = RenderControl::GeometryPassMaterialFlags::SKYBOX;
  
  Network::TextureChangeInfo l_skyText;
  l_skyText.m_id = 1;
  l_skyText.m_textureLayer = 0;
  l_skyText.m_cubeText = true ;
  l_skyText.m_path[0] = std::string("../Assets/Skybox/spacebox/X+.jpg");
  l_skyText.m_path[1] = std::string("../Assets/Skybox/spacebox/X-.jpg");
  l_skyText.m_path[2] = std::string("../Assets/Skybox/spacebox/Y+.jpg");
  l_skyText.m_path[3] = std::string("../Assets/Skybox/spacebox/Y-.jpg");
  l_skyText.m_path[4] = std::string("../Assets/Skybox/spacebox/Z+.jpg");
  l_skyText.m_path[5] = std::string("../Assets/Skybox/spacebox/Z-.jpg");
  
  
  
  // asteroid
  Network::ObjAddInfo l_asteroid;
  l_asteroid.m_id = 2;
  l_asteroid.m_objType = Network::ObjectType::MESH;
  l_asteroid.m_materialFlags = RenderControl::GeometryPassMaterialFlags::EMISSION_MAP;
  l_asteroid.m_meshPath = std::string("../Assets/Models/Asteroid/asteroid.obj");
  
  Network::ObjTransformInfo l_asteroidStartTransform;
  l_asteroidStartTransform.m_id = 2;
  l_asteroidStartTransform.m_transformType = Network::ObjectTransformType::OBJ_POS;
  l_asteroidStartTransform.x = 0;
  l_asteroidStartTransform.y = 0;
  l_asteroidStartTransform.z = -5;
  
  Network::TextureChangeInfo l_asteroidText;
  l_asteroidText.m_id = 2;
  l_asteroidText.m_textureLayer = 0;
  l_asteroidText.m_cubeText = false;
  l_asteroidText.m_path[0] = std::string("../Assets/Models/Asteroid/diffuse.jpg");
  
  
  
  
  Network::NetworkMsgPtr l_msg = std::make_shared<Network::NetworkMsg>();
  
  l_msg->CreateSceneUpdateMsg(
  {glm::vec3(0,0,0), glm::vec3(0,0,-1), glm::vec3(0,1,0) },
  {l_sky, l_asteroid}, {1, 2, 3}, {l_asteroidStartTransform}, {l_asteroidText}, {l_sky, l_asteroid}, {4,5,6}, {l_asteroidStartTransform});
  // {}, {}, {}, {l_skyText}, {}, {}, {});
  // {l_sky, l_asteroid}, {}, {}, {l_skyText}, {}, {}, {});
  
  
  
  std::array<glm::vec3, 3> m_outCameraSettings;
  std::vector<Network::ObjAddInfo> m_outObjsToAdd; 
  std::vector<uint32_t> m_outObjsToRemove;
  std::vector<Network::ObjTransformInfo> m_outObjsToTransform; 
  std::vector<Network::TextureChangeInfo> m_outTextureChange;
  std::vector<Network::ObjAddInfo> m_outLightsToAdd; 
  std::vector<uint32_t> m_outLightsToRemove;
  std::vector<Network::ObjTransformInfo> m_outLightsToTransform;


  l_msg->DeserializeSceneUpdateMsg(m_outCameraSettings, m_outObjsToAdd, m_outObjsToRemove, m_outObjsToTransform,
                                   m_outTextureChange, m_outLightsToAdd, m_outLightsToRemove, m_outLightsToTransform);
  
  if( m_outCameraSettings[0] == glm::vec3(0,0,0)  && m_outCameraSettings[1] == glm::vec3(0,0,-1)  && m_outCameraSettings[2] == glm::vec3(0,1,0) )
      std::cout << "Camera is pass properly" << std::endl;
  if( m_outObjsToAdd.size() == 2 )
  {
    if( m_outObjsToAdd[0].m_id = 1 && m_outObjsToAdd[0].m_objType == Network::ObjectType::SKYBOX && m_outObjsToAdd[0].m_materialFlags == RenderControl::GeometryPassMaterialFlags::SKYBOX )
      std::cout << "object add 1 added properly" << std::endl;
    if( m_outObjsToAdd[1].m_id = 2 && m_outObjsToAdd[1].m_objType == Network::ObjectType::MESH && m_outObjsToAdd[1].m_materialFlags == RenderControl::GeometryPassMaterialFlags::EMISSION_MAP && m_outObjsToAdd[1].m_meshPath == std::string("../Assets/Models/Asteroid/asteroid.obj") )
      std::cout << "object add 2 added properly" << std::endl;
  }
  
  if( m_outObjsToRemove.size() == 3 )
  {
    if( m_outObjsToRemove[0] == 1 && m_outObjsToRemove[1] == 2 && m_outObjsToRemove[2] == 3 )
      std::cout << "object removals added properly" << std::endl;
  }
  
  if( m_outObjsToTransform.size() == 1 )
  {
    if( m_outObjsToTransform[0].m_id == 2 && m_outObjsToTransform[0].m_transformType == Network::ObjectTransformType::OBJ_POS && m_outObjsToTransform[0].x == 0 && m_outObjsToTransform[0].y == 0 && m_outObjsToTransform[0].z == -5 )
      std::cout << "object transform added properly" << std::endl;
  }
  
  if( m_outTextureChange.size() == 1 )
    if( m_outTextureChange[0].m_id == 2 && m_outTextureChange[0].m_textureLayer == 0 && m_outTextureChange[0].m_cubeText == false && m_outTextureChange[0].m_path[0] == std::string("../Assets/Models/Asteroid/diffuse.jpg") )
      std::cout << "texture change added properly" << std::endl;
      
      
      
  if( m_outLightsToAdd.size() == 2 )
  {
    if( m_outLightsToAdd[0].m_id = 1 && m_outLightsToAdd[0].m_objType == Network::ObjectType::SKYBOX && m_outLightsToAdd[0].m_materialFlags == RenderControl::GeometryPassMaterialFlags::SKYBOX )
      std::cout << "ligh add 1 added properly" << std::endl;
    if( m_outLightsToAdd[1].m_id = 2 && m_outLightsToAdd[1].m_objType == Network::ObjectType::MESH && m_outLightsToAdd[1].m_materialFlags == RenderControl::GeometryPassMaterialFlags::EMISSION_MAP && m_outLightsToAdd[1].m_meshPath == std::string("../Assets/Models/Asteroid/asteroid.obj") )
      std::cout << "ligh add 2 added properly" << std::endl;
  }
  
  if( m_outLightsToRemove.size() == 3 )
  {
    if( m_outLightsToRemove[0] == 4 && m_outLightsToRemove[1] == 5 && m_outLightsToRemove[2] == 6 )
      std::cout << "ligh removals added properly" << std::endl;
  }
  
  if( m_outLightsToTransform.size() == 1 )
  {
    if( m_outLightsToTransform[0].m_id == 2 && m_outLightsToTransform[0].m_transformType == Network::ObjectTransformType::OBJ_POS && m_outLightsToTransform[0].x == 0 && m_outLightsToTransform[0].y == 0 && m_outLightsToTransform[0].z == -5 )
      std::cout << "ligh transform added properly" << std::endl;
  }
  
  system("pause");
  return 0;

}
