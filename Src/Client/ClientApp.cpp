/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ClientApp.cpp
 *   DATE: Fri Jun 22 21:22:47 2018
 *  DESCR: 
 ***********************************************************************/
#include "Client/ClientApp.h"
#include "Common/SceneControl/SceneNode.h"
#include "Common/SceneControl/TexturedSceneNode.h"
#include "Common/SceneControl/LightSceneNode.h"

#include "OpenGL/GLGraphicsEngine.h"
#include "Vulkan/VKGraphicsEngine.h"


/***********************************************************************
 *  Method: ClientApp::ClientApp
 *  Params: 
 * Effects: 
 ***********************************************************************/
ClientApp::ClientApp( const std::string &a_hostName, const unsigned int &a_hostPort, const ImplTech& a_implTech)
  // :m_implTech(a_implTech), m_graphics(nullptr), m_dt(0)
  :m_implTech(a_implTech), m_client(nullptr), m_graphics(nullptr), m_dt(0)
{
  m_hasUpdated[0] = true;
  m_hasUpdated[1] = true;
  m_hasUpdated[2] = false;
  
  m_client = new Network::ClientControl(a_hostName, a_hostPort);
}


/***********************************************************************
 *  Method: ClientApp::~ClientApp
 *  Params: 
 * Effects: 
 ***********************************************************************/
ClientApp::~ClientApp()
{
  if( m_client)
    delete m_client;
  if( m_graphics )
   delete m_graphics;
}


/***********************************************************************
 *  Method: ClientApp::SetHinstance
 *  Params: HINSTANCE hinstance
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
ClientApp::SetHinstance(HINSTANCE hinstance)
{
  m_hInstance = hinstance;
}

/***********************************************************************
 *  Method: ClientApp::Execute
 *  Params: 
 * Returns: WPARAM
 * Effects: 
 ***********************************************************************/
WPARAM ClientApp::Execute()
{
  m_pHighResolutionTimer = new CHighResolutionTimer;
  
  if( m_implTech == ImplTech::OPENGL )
  {    
    m_gameWindow.Init(m_hInstance, glm::vec2(10,10) );
    m_graphics = new GLGraphicsEngine();
  }
  else if( m_implTech == ImplTech::VULKAN )  
  {
    VKGraphicsEngine* l_graphics = new VKGraphicsEngine();
    m_graphics = l_graphics;
    m_gameWindow.Init(m_hInstance, glm::vec2(680, 420), l_graphics->GetVkInstance(), l_graphics->GetVkSurface() );
  }
  
  if(!m_gameWindow.Hdc()) {
		return 1;
	}
  
  Initialise();
 
  MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
      }

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} 
    else 
    {
			Update();
		} 
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
  
}



/***********************************************************************
 *  Method: ClientApp::ProcessEvents
 *  Params: HWND window, UINT message, WPARAM w_param, LPARAM l_param
 * Returns: LRESULT
 * Effects: 
 ***********************************************************************/
LRESULT
ClientApp::ProcessEvents(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
  LRESULT result = 0;

	switch (message) {
  case WM_ACTIVATE:
	{
    switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				
				break;
			case WA_INACTIVE:
				
				break;
		}
		break;
	}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
			// if( m_graphics )
        // m_graphics->SetResolution(glm::vec2( glm::abs(dimensions.right - dimensions.left) , glm::abs(dimensions.top - dimensions.bottom) ) );
      
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_KEYUP:
		switch (w_param) {
      case VK_SPACE: 
			 break;
      case 'W': 
			 break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

    default:
      result = DefWindowProc(window, message, w_param, l_param);
		break;
  }
  return result;

}


/***********************************************************************
 *  Method: ClientApp::Initialise
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
ClientApp::Initialise()
{
  // SETUP:
  // 1) SERVER STARTS - READY TO ACCEPT CONNECTIONS
  // 2) CLIENT STARTS - CONNECTS TO SERVER - SENDS REQUEST *
  // 3) SERVER COLLECTS REQUESTS
  // 4) SERVER FINALIZES GETTING REQUESTS
  // 5) SERVER GENERATES AND SENDS RENDERING INFO TO CLIENTS *
  // 6) CLIENTS SETUP ENGINES 
  // 7) CLIENTS SEND CLIENT READY SIGNALS *

  // CONNECTS TO SERVER
  m_client->Connect();
  
  m_client->StartCommunication();
  
  // SENDS REQUEST
  Network::NetworkMsgPtr l_msg = std::make_shared<Network::NetworkMsg>();
  l_msg->CreateClientRequestMsg();
  IFDBG( std::cout << "Send: " << (*l_msg) << std::endl << std::endl; );
  m_client->PushMsg( l_msg );
  
  // wait for a rendering info from the server
  glm::vec4 l_viewport; 
  glm::vec2 l_res;
  glm::vec2 l_partialRes;
  bool l_wait = true;
  while(l_wait)  
  {
    m_client->Update();
    std::vector<Network::NetworkMsgPtr> l_msgs = m_client->GetMsgs();
    // we expect only one message at this point
    if( l_msgs.size() == 1 ) 
      if( l_msgs[0]->GetType() == Network::MsgType::SRV_SETUP )
      {
        l_msgs[0]->DeserializeSetupMsg(l_viewport, l_partialRes, l_res);
        l_wait = false;
        
        IFDBG( std::cout << "Received: " << (*l_msgs[0]) << std::endl << std::endl; );
      }
  }
  
  // SETUP ENGINE
  m_graphics->SetResolution(l_res);
  m_graphics->SetPartRes(l_partialRes);
  m_graphics->SetViewportSettings(l_viewport);
  m_graphics->Init(false); 
  
  // CLIENT SEND CLIENT READY SIGNAL
  l_msg = std::make_shared<Network::NetworkMsg>();
  l_msg->CreateEngineReadyMsg();
  m_client->PushMsg(l_msg);
  
  m_camera = m_graphics->GetSceneManager()->AddCameraSceneNode(m_graphics->GetDeferredRenderPass()->GetCamera(), SceneControl::STATIC_CAMERA);
  
  IFDBG( std::cout << "send: " << (*l_msg) << std::endl << std::endl; );
  
}


/***********************************************************************
 *  Method: ClientApp::Update
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
ClientApp::Update()
{
  // IFDBG( std::cout << "Loop start" << std::endl; );
  m_pHighResolutionTimer->Start();
  // 1) SERVER SENDS SCENE UPDATE MESSAGE
  // 2) CLIENTS UPDATE SCENE
  // 3) CLIENTS RENDER SCENE (GEOMETRY PASS)
  // 4) CLIENTS RENDER LIGHTS
  // 5) CLIENTS SEND BACK THE RENDERED TEXTURES *
  // 6) SERVER RENDERS THE RESULT ON SCREEN
  
  // clear all update vectors
  m_outObjsToAdd.clear();
  m_outObjsToRemove.clear();
  m_outObjsToTransform.clear();
  m_outTextureChange.clear();
  m_outLightsToAdd.clear();
  m_outLightsToRemove.clear();
  m_outLightsToTransform.clear();
  
  // CLIENTS UPDATE
  // IFDBG( std::cout << "Client Update Start - "; );
  m_client->Update();
  std::vector<Network::NetworkMsgPtr> l_msgs = m_client->GetMsgs();
  // update scene as appropriate
  m_hasUpdated[ m_hasUpdated[2] ] = false;
      
  for( std::vector<Network::NetworkMsgPtr>::const_iterator l_iter = l_msgs.cbegin(); l_iter != l_msgs.cend(); ++l_iter)
  {
    if( (*l_iter)->GetType() == Network::MsgType::SRV_SCENE_UPDATE )
    {
      m_hasUpdated[ m_hasUpdated[2] ] = true;
      (*l_iter)->DeserializeSceneUpdateMsg(m_outCameraSettings, m_outObjsToAdd, m_outObjsToRemove, m_outObjsToTransform,
                                           m_outTextureChange, m_outLightsToAdd, m_outLightsToRemove, m_outLightsToTransform);
      // std::cout << (**l_iter) << std::endl;
      SetCamera(m_outCameraSettings[0], m_outCameraSettings[1], m_outCameraSettings[2]);
      
      // objects to add
      for( std::vector<Network::ObjAddInfo>::iterator l_objsToAdd = m_outObjsToAdd.begin(); l_objsToAdd != m_outObjsToAdd.end(); ++l_objsToAdd)
        AddObject(*l_objsToAdd, m_outTextureChange);
      // objects to remove
      for( std::vector<uint32_t>::iterator l_objsToRemove = m_outObjsToRemove.begin(); l_objsToRemove != m_outObjsToRemove.end(); ++l_objsToRemove)
        RemoveObject(*l_objsToRemove, false);
      // objects to transform
      for( std::vector<Network::ObjTransformInfo>::iterator l_objsToTrans = m_outObjsToTransform.begin(); l_objsToTrans != m_outObjsToTransform.end(); ++l_objsToTrans)
        TransformObject(*l_objsToTrans, false);
      
      // // lights to add
      for( std::vector<Network::ObjAddInfo>::iterator l_lightsToAdd = m_outLightsToAdd.begin(); l_lightsToAdd != m_outLightsToAdd.end(); ++l_lightsToAdd)
      {
        AddLight(*l_lightsToAdd);
      }
      // lights to remove
      for( std::vector<uint32_t>::iterator l_lightsToRemove = m_outLightsToRemove.begin(); l_lightsToRemove != m_outLightsToRemove.end(); ++l_lightsToRemove)
        RemoveObject(*l_lightsToRemove, true);
      for( std::vector<Network::ObjTransformInfo>::iterator l_lightsToTrans = m_outLightsToTransform.begin(); l_lightsToTrans != m_outLightsToTransform.end(); ++l_lightsToTrans)
        TransformObject(*l_lightsToTrans, true);
    }
    
      
  }
  // IFDBG( std::cout << "Scene Update" << std::endl; );
  
  // CLIENTS RENDER SCENE (GEOMETRY PASS)
  // CLIENTS RENDER LIGHTS
  if( m_hasUpdated[ !m_hasUpdated[2] ] )
  {
    try
    {
      m_graphics->Update(1);
    }
    catch (std::runtime_error& e)
    {
      std::cout << e.what();
    }
    // IFDBG( std::cout << "Render Update" << std::endl; );
    
    // CLIENTS SEND BACK THE RENDERED TEXTURES
    
    Network::NetworkMsgPtr l_test = std::make_shared<Network::NetworkMsg>();
    
    if( m_graphics->GetDeferredRenderPass()->PackTexture(l_test) )
    {
      m_client->PushMsg(l_test);
      m_frameCount++;
    }
  }
  m_hasUpdated[2] = !m_hasUpdated[2];
  // IFDBG( std::cout << "Send Render Result Update" << std::endl; );
  
  m_dt = m_pHighResolutionTimer->Elapsed();
  
  // framerate output
  
  m_elapsedTime += m_dt;
  // Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
  {
		printf( "FPS: %f\n", (m_frameCount*1000)/m_elapsedTime );
		m_elapsedTime = 0;
		// Reset the frames per second
		m_frameCount = 0;
  }
  
  // IFDBG( std::cout << "Loop End" << std::endl<< std::endl; );
  
}


void ClientApp::SetCamera(glm::vec3 a_pos, glm::vec3 a_view, glm::vec3 a_up)
{
  // std::cout << a_pos.x << std::endl;
  m_camera->GetCamera()->Set(a_pos, a_view, a_up); 
}

/***********************************************************************
 *  Method: ClientApp::AddObject
 *  Params: const Network::ObjAddInfo& a_info
 * Returns: void
 * Effects: 
 ***********************************************************************/
void 
ClientApp::AddObject(const Network::ObjAddInfo& a_info, const std::vector<Network::TextureChangeInfo>& a_textures)
{
  if( a_info.m_id == 0 )
    return;
  

  
  
  std::shared_ptr<IMesh> l_mesh;
  // get the appropriate shape first
  switch(a_info.m_objType)
  {
    case Network::ObjectType::CUBE: 
      l_mesh = m_graphics->GetShapeFactory()->GetCube();
      break;
    case Network::ObjectType::SKYBOX: 
      l_mesh = m_graphics->GetShapeFactory()->GetSkybox();
      break;
    case Network::ObjectType::SPHERE: 
      l_mesh = m_graphics->GetShapeFactory()->GetSphere();
      break;
    case Network::ObjectType::MESH:
      l_mesh = m_graphics->GetShapeFactory()->GetOpenAssetImportMesh(a_info.m_meshPath);
      break;
  }
  
  if(l_mesh)
  {
    SceneControl::MeshSceneNode* l_meshNode = m_graphics->GetSceneManager()->AddMeshSceneNode( l_mesh );
    if( l_meshNode->SetID(a_info.m_id) )
    {
      // texture set before adding it to the renderpass 
      for( std::vector<Network::TextureChangeInfo>::const_iterator l_textChange = a_textures.cbegin(); l_textChange != a_textures.cend(); ++l_textChange)
        if( l_textChange->m_id == a_info.m_id )
        {
          TextureChange(*l_textChange);
        }
      m_graphics->GetDeferredRenderPass()->AddRenderable(l_meshNode, (RenderControl::GeometryPassMaterialFlags)(a_info.m_materialFlags));
      

    }
    else // if bad id given remove it from the registry and detach it from the scene manager
    {
      l_meshNode->SetID(0);
      l_meshNode->GetRoot()->DetachDescendant(l_meshNode);
    }
  }
}

void 
ClientApp::RemoveObject(const uint32_t& a_id, bool a_isLight)
{
  SceneControl::SceneNode* l_node = SceneControl::SceneNode::GetByID(a_id);
  if( l_node )
  {
    l_node->SetID(0);
    l_node->GetRoot()->DetachDescendant(l_node);
    if( a_isLight ) 
      m_graphics->GetDeferredRenderPass()->RemoveLight(l_node);
    else 
      m_graphics->GetDeferredRenderPass()->RemoveRenderable(l_node);
  }
}

/***********************************************************************
 *  Method: ClientApp::TransformObject
 *  Params: const Network::ObjTransformInfo& a_info
 * Returns: void
 * Effects: 
 ***********************************************************************/
void 
ClientApp::TransformObject(const Network::ObjTransformInfo& a_info, bool a_isLight)
{
  SceneControl::SceneNode* l_node = SceneControl::SceneNode::GetByID(a_info.m_id);
  if( l_node )
  {
    switch(a_info.m_transformType)
    {
      case Network::ObjectTransformType::OBJ_POS: 
      l_node->SetPos(glm::vec3(a_info.x,a_info.y,a_info.z));
      break;
      case Network::ObjectTransformType::OBJ_ROT:
      l_node->SetEulerAngles(glm::vec3(a_info.x,a_info.y,a_info.z));
      break;
      case Network::ObjectTransformType::OBJ_SCALE: 
      if( !a_isLight )
        l_node->SetScale(glm::vec3(a_info.x,a_info.y,a_info.z));
      break;
      case Network::ObjectTransformType::LGHT_AMBIENT:
      if( a_isLight )
        ( (SceneControl::LightSceneNode*)l_node)->SetAmbient(glm::vec3(a_info.x,a_info.y,a_info.z));
      break;
      case Network::ObjectTransformType::LGHT_DIFFUSE: 
      if( a_isLight )
        ( (SceneControl::LightSceneNode*)l_node)->SetDiffuse(glm::vec3(a_info.x,a_info.y,a_info.z));
      break;
      case Network::ObjectTransformType::LGHT_SPECULAR: 
      if( a_isLight )
        ((SceneControl::LightSceneNode*)l_node)->SetSpecular(glm::vec3(a_info.x,a_info.y,a_info.z));
      break;
      case Network::ObjectTransformType::LGHT_ATTENUATION: 
      if( a_isLight )
      {
        ((SceneControl::LightSceneNode*)l_node)->SetConstAttenuation(a_info.x);
        ((SceneControl::LightSceneNode*)l_node)->SetLinAttenuation(a_info.y);
        ((SceneControl::LightSceneNode*)l_node)->SetQuadAttenuation(a_info.z);
      }
      break;
    }
  }
}
/***********************************************************************
 *  Method: ClientApp::TextureChange
 *  Params: const Network::TextureChangeInfo& a_info
 * Returns: void
 * Effects: 
 ***********************************************************************/
void ClientApp::TextureChange(const Network::TextureChangeInfo& a_info)
{
  SceneControl::TexturedSceneNode* l_node = dynamic_cast<SceneControl::TexturedSceneNode*>( SceneControl::SceneNode::GetByID(a_info.m_id) );
  if( l_node )
  {
    if( a_info.m_cubeText)
    {
      l_node->SetTexture(a_info.m_textureLayer, m_graphics->GetTextureFactory()->GetCubemap(a_info.m_path[0], a_info.m_path[1], a_info.m_path[2], a_info.m_path[3], a_info.m_path[4], a_info.m_path[5]) );
    }
    else
      l_node->SetTexture(a_info.m_textureLayer, m_graphics->GetTextureFactory()->GetTexture(a_info.m_path[0]) );
  }
}


/***********************************************************************
 *  Method: ClientApp::AddLight
 *  Params: const Network::ObjAddInfo& a_info
 * Returns: void
 * Effects: 
 ***********************************************************************/

void ClientApp::AddLight(const Network::ObjAddInfo& a_info)
{
  if( a_info.m_id == 0 )
    return;
  
  SceneControl::LightSceneNode* l_light = nullptr;
  // get the appropriate shape first
  switch(a_info.m_lightFlags)
  {
    case RenderControl::LightTypeFlags::POINT_LIGHT: 
    {
      std::shared_ptr<ASphere> l_mesh = m_graphics->GetShapeFactory()->GetSphere();
      l_light = m_graphics->GetSceneManager()->AddPointLightSceneNode( l_mesh );
      ( (SceneControl::PointLightSceneNode*)l_light )->Init();
    }
    break;
    case RenderControl::LightTypeFlags::SPOT_LIGHT: 
    {
      std::shared_ptr<ACone> l_mesh = m_graphics->GetShapeFactory()->GetCone();
      l_light = m_graphics->GetSceneManager()->AddSpotLightSceneNode( l_mesh );
      ( (SceneControl::SpotLightSceneNode*)l_light )->Init();
    }
    break;
    case RenderControl::LightTypeFlags::DIRECTIONAL_LIGHT: 
    {
      std::shared_ptr<ARect> l_mesh = m_graphics->GetShapeFactory()->GetRectangle();
      l_light = m_graphics->GetSceneManager()->AddDirectionalLightSceneNode( l_mesh );
      ( (SceneControl::DirectionalLightSceneNode*)l_light )->Init();
    }
    break;
  }
  if( l_light )
  {
    if(  l_light->SetID(a_info.m_id))
      m_graphics->GetDeferredRenderPass()->AddLight(l_light, a_info.m_lightFlags);
    else // if bad id given remove it from the registry and detach it from the scene manager
    {
      l_light->SetID(0);
      l_light->GetRoot()->DetachDescendant(l_light);
    }
    
  }
}
