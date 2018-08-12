/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ServerApp.cpp
 *   DATE: Mon Jun 18 18:06:49 2018
 *  DESCR: 
 ***********************************************************************/
#include "Server/ServerApp.h"
#include "Common/SceneControl/MeshSceneNode.h"

#include "OpenGL/GLGraphicsEngine.h"
#include "Vulkan/VKGraphicsEngine.h"

#include "Common/Core/MyUtilities.h"
#include "Common/Textures/ATexture.h"
#include <algorithm>

#include <vulkan/vulkan.h>

#include "Server/SceneOne.h"
#include "Server/SceneTwo.h"

#include <fstream>
/***********************************************************************
 *  Method: ServerApp::ServerApp
 *  Params: 
 * Effects: 
 ***********************************************************************/
ServerApp::ServerApp(const glm::vec2& a_dimensions, const unsigned int& a_port, const ImplTech& a_implTech, const unsigned int& a_clientsCount, const unsigned int& a_testIndex, 
                     const std::string& a_outFile, const unsigned int& a_numberOfLights)
  : m_appActive(true), m_elapsedTime(0), m_elapsedTime2(0), m_dt(0), m_frameCount(0), m_dimensions(a_dimensions), m_pHighResolutionTimer(nullptr), m_graphics(nullptr), m_serverCtrl(a_port), m_implTech(a_implTech),
    m_clientsCount(a_clientsCount), m_testIndex(a_testIndex), m_outFile(a_outFile), m_numberOfLights(a_numberOfLights)
{
  m_textureData = new char[m_dimensions.x * m_dimensions.y  * 4];
}


/***********************************************************************
 *  Method: ServerApp::~ServerApp
 *  Params: 
 * Effects: 
 ***********************************************************************/
ServerApp::~ServerApp()
{

  if( m_pHighResolutionTimer)
    delete m_pHighResolutionTimer;
  if( m_graphics )
    delete m_graphics;
  if( m_textureData )
    delete[] m_textureData;
 
}


/***********************************************************************
 *  Method: ServerApp::ProcessEvents
 *  Params: HWND window, UINT message, WPARAM w_param, LPARAM l_param
 * Returns: LRESULT
 * Effects: 
 ***********************************************************************/
LRESULT
ServerApp::ProcessEvents(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
  LRESULT result = 0;

	switch (message) {
  case WM_ACTIVATE:
	{
    switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				// m_appActive = true;
				//m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				// m_appActive = false;
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
      {
        std::fstream fs;
        fs.open (m_outFile, std::fstream::out);
        fs << m_outFile << std::endl;
        fs << m_output.str();
        fs.close();
      }
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
    {
      std::fstream fs;
      fs.open (m_outFile, std::fstream::out);
      fs << m_outFile << std::endl;
      fs << m_output.str();
      fs.close();
    }
    PostQuitMessage(0);
		break;

    default:
      result = DefWindowProc(window, message, w_param, l_param);
		break;
  }
  return result;

}


/***********************************************************************
 *  Method: ServerApp::SetHinstance
 *  Params: HINSTANCE hinstance
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
ServerApp::SetHinstance(HINSTANCE hinstance)
{
  m_hInstance = hinstance;
}


/***********************************************************************
 *  Method: ServerApp::Execute
 *  Params: 
 * Returns: WPARAM
 * Effects: 
 ***********************************************************************/
WPARAM ServerApp::Execute()
{

  m_pHighResolutionTimer = new CHighResolutionTimer;
  
  if( m_implTech == ImplTech::OPENGL )
  {    
    m_gameWindow.Init(m_hInstance, m_dimensions );
    m_graphics = new GLGraphicsEngine();
  }
  else if( m_implTech == ImplTech::VULKAN )  
  {
    VKGraphicsEngine* l_graphics = new VKGraphicsEngine();
    m_graphics = l_graphics;
    
    // it receives the vk instance and it will create a vk surface
    m_gameWindow.Init(m_hInstance, m_dimensions, l_graphics->GetVkInstance(), l_graphics->GetVkSurface() );
  }
  
  if(!m_gameWindow.Hdc()) {
		return 1;
	}
  RECT dimensions = m_gameWindow.GetDimensions();
  // std::cout << "initializing with " << glm::abs(dimensions.right - dimensions.left) << " " << glm::abs(dimensions.top - dimensions.bottom);
  m_graphics->SetResolution( glm::vec2( glm::abs(dimensions.right - dimensions.left) , glm::abs(dimensions.top - dimensions.bottom) ) );
  m_graphics->SetPartRes( glm::vec2( glm::abs(dimensions.right - dimensions.left) , glm::abs(dimensions.top - dimensions.bottom) ) );
  m_graphics->Init(true, m_clientsCount);
  
  
  Initialise();
  
  m_pHighResolutionTimer->Start();
  
  MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			Update();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
  
}


/***********************************************************************
 *  Method: ServerApp::Update
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
ServerApp::Update()
{
  
  m_pHighResolutionTimer->Start();
  
  /////////////////////////////////////////////
  // do here what need to be done /////////////
  /////////////////////////////////////////////
  // 1) SERVER SENDS SCENE UPDATE MESSAGE
  // 2) CLIENTS UPDATE SCENE
  // 3) CLIENTS RENDER SCENE (GEOMETRY PASS)
  // 4) CLIENTS RENDER LIGHTS
  // 5) CLIENTS SEND BACK THE RENDERED TEXTURES *
  // 6) SERVER RENDERS THE RESULT ON SCREEN
  
  // SERVER SENDS SCENE UPDATE MESSAGE
  
  m_serverCtrl.Update();
  
  if( m_socketsToCompleteFrame.size() == 0 )
    m_socketsToCompleteFrame = m_sockets;
  
  const std::vector< RenderControl::CompositionEntity >& l_compEntity = m_graphics->GetCompositionPass()->GetSubpartsSettings();
  std::vector< SceneControl::MeshSceneNode* >& l_rects = m_graphics->GetCompositionPass()->GetSubpartsRects();
  
  std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<Network::NetworkMsgPtr> > l_msgs = m_serverCtrl.GetMsgs();
  for( std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<Network::NetworkMsgPtr> >::iterator l_iter = l_msgs.begin(); l_iter != l_msgs.end(); ++l_iter)
  {
    // for each socket
    // check the last message
    Network::NetworkMsgPtr l_msg = l_iter->second.back();
    // IFDBG( std::cout << "Received Message of type" << *l_msg << std::endl; );
    if( l_msg->GetType() == Network::MsgType::CLNT_RENDER_RESULT )
    {
      
      m_socketsToCompleteFrame.erase(l_iter->first);
      uint32_t l_textureSize;
      glm::vec2 l_resolution;
      unsigned int l_colourType;
      unsigned int l_outBitDepth;
      if( l_msg->DeserializeRenderResultMsg(m_textureData, l_resolution, l_colourType, l_outBitDepth) )
      {
        unsigned int l_index = m_clients[l_iter->first];
        std::shared_ptr<ATexture> l_text = std::dynamic_pointer_cast< ATexture > ( l_rects[l_index]->GetTexture(0) );
        if( l_text )
        {
          l_text->UpdateData(m_textureData, l_resolution.x, l_resolution.y, 32, false);
        }
      }
    }
  }
  
  m_graphics->Update(m_dt);
  // system("pause");
  m_dt = m_pHighResolutionTimer->Elapsed();
  
  // framerate output
  m_frameCount++;
  m_elapsedTime += m_dt;
  m_elapsedTime2 += m_dt;
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // we remove this - find a better way
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (m_socketsToCompleteFrame.size() == 0)
  {
    UpdateScene();
  }
  // Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
  {
		printf( "%f\n", (m_frameCount*1000)/m_elapsedTime );
    if( m_frameCount )
      m_output << m_elapsedTime/m_frameCount << std::endl;
    
    m_elapsedTime = 0;
		// Reset the frames per second
		m_frameCount = 0;
  }
  
  // Swap buffers to show the rendered image
  if( m_implTech == ImplTech::OPENGL)
    SwapBuffers(m_gameWindow.Hdc());
}


void
ServerApp::UpdateScene()
{
  m_sceneController->Update(m_elapsedTime2);
  m_elapsedTime2 = 0;
}

/***********************************************************************
 *  Method: ServerApp::Initialise
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
ServerApp::Initialise()
{

  
  /////////////////////////////////////////////
  // do here what need to be done /////////////
  /////////////////////////////////////////////
  
  // SETUP:
  // 1) SERVER STARTS - READY TO ACCEPT CONNECTIONS
  // 2) CLIENT STARTS - CONNECTS TO SERVER - SENDS REQUEST *
  // 3) SERVER COLLECTS REQUESTS
  // 4) SERVER FINALIZES GETTING REQUESTS
  // 5) SERVER GENERATES AND SENDS RENDERING INFO TO CLIENTS *
  // 6) CLIENTS SETUP ENGINES 
  // 7) CLIENTS SEND CLIENT READY SIGNALS *
  
  // SERVER STARTS - READY TO ACCEPT CONNECTIONS
  m_serverCtrl.AcceptConnections();
  while( m_serverCtrl.GetConnectedClientsCount() < m_clientsCount )
    m_serverCtrl.Update();

  std::vector<std::shared_ptr<asio::ip::tcp::socket> > l_sockets = m_serverCtrl.StopAcceptingConnections();
  m_sockets = std::set<std::shared_ptr<asio::ip::tcp::socket> >(l_sockets.begin(), l_sockets.end());
  

  m_serverCtrl.StartClientCommunication();
  
  // SERVER COLLECTS REQUESTS - collecting a request from a specific socket indicates its a rendering client
  std::vector<std::shared_ptr<asio::ip::tcp::socket> > l_clients;
  while( l_clients.size() < m_clientsCount )
  {
    m_serverCtrl.Update();
    // get any new messages
    std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<Network::NetworkMsgPtr> > l_msgs = m_serverCtrl.GetMsgs();
    // for each socket that has sent a message
    for( std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<Network::NetworkMsgPtr> >::iterator l_iter = l_msgs.begin(); l_iter != l_msgs.end(); ++l_iter)
      // if this socket is not registered and
      if( std::find(l_clients.begin(), l_clients.end(), l_iter->first ) == l_clients.end() )   
        // there is only one message from each socket, a Network::MsgType::CLNT_REQUEST
        if( l_iter->second.size() == 1 )
          if( l_iter->second[0]->GetType() == Network::MsgType::CLNT_REQUEST )
          {
            // register the socket
            l_clients.push_back(l_iter->first);
            std::cout << "Registered client " << l_iter->first << std::endl;
          }
  }
  
  
  
  IFDBG( std::cout << "Finalize request collection... " << std::endl << std::endl; );
  IFDBG( std::cout << "Initialise graphics engine... " << std::endl << std::endl;; );
  IFDBG( std::cout << "Send: " << std::endl; );
  // SERVER GENERATES AND SENDS RENDERING INFO TO CLIENTS
  const std::vector< RenderControl::CompositionEntity >& l_compositionSettings = m_graphics->GetCompositionPass()->GetSubpartsSettings();
  for( unsigned int i = 0; i < l_compositionSettings.size(); ++i)
  {
    Network::NetworkMsgPtr l_msg = std::make_shared<Network::NetworkMsg>();
    l_msg->CreateSetupMsg( l_compositionSettings[i].m_viewport ,l_compositionSettings[i].m_resolution, m_graphics->GetResolution() );
    
    IFDBG( std::cout << "Send: " << (*l_msg) << std::endl << "to " << l_clients[i] << " and assign them the index " << i << "." << std::endl ; );
    m_serverCtrl.PushMsg(l_clients[i], l_msg);
    m_clients[l_clients[i]] = i;
    
  }
  
  
  // // wait to receive ready signals from all clients
  std::set<std::shared_ptr<asio::ip::tcp::socket>> l_clientsSet(l_clients.begin(), l_clients.end());
  while( l_clientsSet.size() > 0 )
  {
    m_serverCtrl.Update();
    // get any new messages
    std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<Network::NetworkMsgPtr> > l_msgs = m_serverCtrl.GetMsgs();
    // for each socket that has sent a message
    for( std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<Network::NetworkMsgPtr> >::iterator l_iter = l_msgs.begin(); l_iter != l_msgs.end(); ++l_iter)
      // if this socket is registered and
      if( l_clientsSet.find( l_iter->first ) != l_clientsSet.end() )
        // there is only one message from each socket, a Network::MsgType::CLNT_ENGINE_READY
        if( l_iter->second.size() == 1 )
          if( l_iter->second[0]->GetType() == Network::MsgType::CLNT_ENGINE_READY )
          {
            // register the socket
            IFDBG( std::cout << "Client: " << l_iter->first << "is ready to render." << std::endl; );
            l_clientsSet.erase(l_iter->first);
          }
  }
  IFDBG( std::cout << "All clients ready." << std::endl; );
  
  
  std::shared_ptr<CCamera> l_camera = std::make_shared<CCamera>();
  l_camera->SetDimentsions( m_graphics->GetResolution() );
  l_camera->Set(glm::vec3(0,0,0), glm::vec3(0,0,-1), glm::vec3(0,1,0) );
  m_camera = m_graphics->GetSceneManager()->AddCameraSceneNode(l_camera);
  m_camera->SetPos( glm::vec3(0,0,0) );
  IFDBG( std::cout << "Camera ready." << std::endl; );
  
  
  if( m_testIndex == 0)
    m_sceneController = std::make_shared<SceneOne>(m_serverCtrl, m_graphics, l_sockets, m_camera, m_numberOfLights);
  else 
    m_sceneController = std::make_shared<SceneTwo>(m_serverCtrl, m_graphics, l_sockets, m_camera, m_numberOfLights);
  m_sceneController->Init();
}
