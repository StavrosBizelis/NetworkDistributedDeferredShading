/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ClientApp.cpp
 *   DATE: Fri Jun 22 21:22:47 2018
 *  DESCR: 
 ***********************************************************************/
#include "Client/ClientApp.h"
#include "OpenGL/GLGraphicsEngine.h"


/***********************************************************************
 *  Method: ClientApp::ClientApp
 *  Params: 
 * Effects: 
 ***********************************************************************/
ClientApp::ClientApp( const std::string &a_hostName, const unsigned int &a_hostPort, const ImplTech& a_implTech)
  :m_implTech(a_implTech), m_client(a_hostName, a_hostPort), m_graphics(nullptr)
{

}


/***********************************************************************
 *  Method: ClientApp::~ClientApp
 *  Params: 
 * Effects: 
 ***********************************************************************/
ClientApp::~ClientApp()
{
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

  m_gameWindow.Init(m_hInstance, glm::vec2(10,10) );
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

   // SETUP ENGINE
   // glm::vec4 l_viewport; 
  // glm::vec2 l_res;
  // glm::vec2 l_partialRes;
  // if( m_implTech == ImplTech::OPENGL )  
    // m_graphics = new GLGraphicsEngine( l_res, l_partialRes, l_viewport );
  // m_graphics->Init();
  // system("pause");
  // CONNECTS TO SERVER
  m_client.Connect();
  
  m_client.StartCommunication();
  
  // SENDS REQUEST
  Network::NetworkMsgPtr l_msg = std::make_shared<Network::NetworkMsg>();
  l_msg->CreateClientRequestMsg();
  IFDBG( std::cout << "Send: " << (*l_msg) << std::endl << std::endl; );
  m_client.PushMsg( l_msg );
  
  // wait for a rendering info from the server
  glm::vec4 l_viewport; 
  glm::vec2 l_res;
  glm::vec2 l_partialRes;
  bool l_wait = true;
  while(l_wait)  
  {
    m_client.Update();
    std::vector<Network::NetworkMsgPtr> l_msgs = m_client.GetMsgs();
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
  if( m_implTech == ImplTech::OPENGL )  
    m_graphics = new GLGraphicsEngine( l_res, l_partialRes, l_viewport );
  m_graphics->Init(); 
  
  // CLIENT SEND CLIENT READY SIGNAL
  l_msg = std::make_shared<Network::NetworkMsg>();
  l_msg->CreateEngineReadyMsg();
  m_client.PushMsg(l_msg);
  
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
  m_client.Update();
}


