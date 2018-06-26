/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ACompositionPass.cpp
 *   DATE: Tue Jun 26 18:08:43 2018
 *  DESCR: 
 ***********************************************************************/
#include "Common/RenderControl/ACompositionPass.h"
#include "Common/Core/MyUtilities.h"
#include <cmath>
#include <iostream>
/***********************************************************************
 *  Method: RenderControl::ACompositionPass::ACompositionPass
 *  Params: const glm::vec2 &a_resolution, const unsigned int &a_subparts
 * Effects: 
 ***********************************************************************/
RenderControl::ACompositionPass::ACompositionPass(const glm::vec2 &a_resolution, SceneControl::SceneManager* a_scnManager, IShapeFactory* a_shapeFactory, const unsigned int &a_subparts)
  :IRenderPass(a_resolution), m_scnManager(a_scnManager)
{
  m_subpartsSettings.resize(a_subparts);
  
  
  std::shared_ptr<ARect> l_rec = a_shapeFactory->GetRectangle();
  for( unsigned int i =0; i < a_subparts; ++i)
    m_subpartRects.push_back( m_scnManager->AddMeshSceneNode(l_rec) );
  
  UpdateSubpartsSettings();
}


/***********************************************************************
 *  Method: RenderControl::ACompositionPass::~ACompositionPass
 *  Params: 
 * Effects: 
 ***********************************************************************/
RenderControl::ACompositionPass::~ACompositionPass()
{
  for( unsigned int i =0; i < m_subpartRects.size(); ++i)
    m_scnManager->DetachNode( m_subpartRects[i] );
  m_subpartRects.clear();
}

/***********************************************************************
 *  Method: RenderControl::ACompositionPass::UpdateSubpartsSettings
 *  Params: 
 * Returns: void
 * Effects: 
 ***********************************************************************/
void 
RenderControl::ACompositionPass::UpdateSubpartsSettings()
{
  
  // the idea of this algorithm is the following
  // ex: lets say we want to divide 1024 by 3 to as similar as possible integer numbers
  //     what we could do is take 1024/3 = 341.3333 and round it. However that would introduce error and we would lose one pixel
  //     Instead what we will do is the following:
  //     * floor(1024/3) = 341
  //     * floor( (1024-341) / 2 ) = floor(683/2) = 341
  //     * floor( (683-341) / 1 ) = floor ( 342/1) = 342
  //     *** 341+341+342 = 1024 - we lose no pixels
  // We use this to claulate the height of each row, and the width of its block
  
  
  unsigned int l_countToGo = m_subpartsSettings.size();
  unsigned int l_rows = std::floor( sqrt(l_countToGo) );
  
  
  unsigned int l_remainingHeight = (unsigned int)m_resolution.y;
  int l_shiftY = 0;
  unsigned int l_index = 0;
  for( unsigned int i = l_rows; i > 0; --i)
  {
    unsigned int l_countThisRow = l_countToGo/i; // both are int so we get the floor of the division
    unsigned int l_currentHeight = l_remainingHeight/i; // both are int so we get the floor of the division
    int l_shiftX = 0;
    
    unsigned int l_remainingWidth = (unsigned int)m_resolution.x;
    
    for( unsigned int j = l_countThisRow; j > 0; --j)
    {
      unsigned int l_currentWidth = l_remainingWidth/j;
      
      // set the viewport settings and the resolution for this block
      CompositionEntity l_entity;
      l_entity.m_resolution = glm::vec2(l_currentHeight, l_currentWidth);
      l_entity.m_viewport = glm::vec4(-l_shiftX, -l_shiftY, l_currentWidth, l_currentHeight);
      m_subpartsSettings[l_index] = std::move(l_entity);
      // set the position and scale of the current block rectangle
      m_subpartRects[l_index]->SetPos( glm::vec3( 
                                            /*m_resolution.x */ ( (l_currentWidth/2)+l_shiftX),
                                            /*m_resolution.y */ ( (l_currentHeight/2)+l_shiftY), 1 ) );
      
      m_subpartRects[l_index]->SetScale( glm::vec3(
                                            m_resolution.x * ( (float)l_currentWidth/m_resolution.x ), 
                                            m_resolution.y * ( (float)l_currentHeight/m_resolution.y ), 1 ) );
      ++l_index;

      IFDBG( 
          std::cout << "test: " << m_resolution.x <<  " " << l_currentWidth/2 << " " << l_shiftX << std::endl;
          std::cout << "rect: " << /*m_resolution.x */ ( (l_currentWidth/2)+l_shiftX) << ", " <<  /*m_resolution.y */ ( (l_currentHeight/2)+l_shiftY) << std::endl;
          std::cout << "scale: " << /*m_resolution.x */ ( (float)l_currentWidth/m_resolution.x ) << ", " << /*m_resolution.y */ ( (float)l_currentHeight/m_resolution.y ) << std::endl;
      );
      
      l_shiftX += l_currentWidth;
      l_remainingWidth -= l_currentWidth;
    }
    
    l_shiftY += l_currentHeight;
    l_remainingHeight -= l_currentHeight;
    l_countToGo -= l_countThisRow;
  }
  
  // IFDBG(
    // std::cout << "Composition pass resolution: " << m_resolution.x << ", " << m_resolution.y << std::endl;
    // for(unsigned int i =0; i < m_subpartsSettings.size(); ++i)
    // {
      // std::cout << "block :"<<i << std::endl <<"viewport settings :"<<m_subpartsSettings[i].m_viewport.x << "," << m_subpartsSettings[i].m_viewport.y << "," << m_subpartsSettings[i].m_viewport.z << "," << m_subpartsSettings[i].m_viewport.w << std::endl;
    // }  
  // );
}

/***********************************************************************
 *  Method: RenderControl::ACompositionPass::GetSubpartsSettings
 *  Params: 
 * Returns: const std::vector<CompositionEntity> &
 * Effects: 
 ***********************************************************************/
const std::vector<RenderControl::CompositionEntity> &
RenderControl::ACompositionPass::GetSubpartsSettings() const
{
  return m_subpartsSettings;
}


/***********************************************************************
 *  Method: RenderControl::ACompositionPass::SetMaterialManager
 *  Params: MaterialControl::IMaterialManager *a_materialManager
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
RenderControl::ACompositionPass::SetMaterialManager(MaterialControl::IMaterialManager *a_materialManager)
{
  IRenderPass::SetMaterialManager(a_materialManager);
}