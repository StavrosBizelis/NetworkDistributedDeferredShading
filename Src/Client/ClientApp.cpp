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
ClientApp::ClientApp( const glm::vec2& a_dimensions, const ImplTech& a_implTech)
  :m_implTech(a_implTech), m_graphics(nullptr), m_client(nullptr)
{
  if( m_implTech == ImplTech::OPENGL )
    m_graphics = new GLGraphicsEngine(a_dimensions);
}


/***********************************************************************
 *  Method: ClientApp::~ClientApp
 *  Params: 
 * Effects: 
 ***********************************************************************/
ClientApp::~ClientApp()
{
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
  m_graphics->Init();
  
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
}


