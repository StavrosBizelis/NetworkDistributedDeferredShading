/***********************************************************************
 * AUTHOR:  <StavrosBizelis>
 *   FILE: NetworkMsg.cpp
 *   DATE: Fri Jun 15 13:26:36 2018
 *  DESCR: 
 ***********************************************************************/
#include "Common/Network/NetworkMsg.h"
#include "Common/Core/MyUtilities.h"
namespace Network
{
  
  
  
  void ObjAddInfo::Serialize(char* a_arrOut) const
  {
    ConsistentInt32ToCharArray(m_id, a_arrOut);
    a_arrOut+=4;
    memcpy(a_arrOut, (void*)&m_objType, 1);
    a_arrOut+=1;
    ConsistentInt32ToCharArray(m_materialFlags, a_arrOut);
    a_arrOut+=4;
    memcpy(a_arrOut, (void*)&m_lightFlags, 1);
    a_arrOut+=1;
    ConsistentInt32ToCharArray(m_meshPath.size(), a_arrOut);
    a_arrOut+=4;
    memcpy(a_arrOut, &(m_meshPath[0]), m_meshPath.length());
    a_arrOut += m_meshPath.length();
  }
  
  void ObjAddInfo::Deserialize(char* a_arrIn)
  {
    m_id = ConsistentCharArrToInt32(a_arrIn);
    a_arrIn+=4;
    m_objType = (ObjectType)(*a_arrIn);
    a_arrIn+=1;
    m_materialFlags = (RenderControl::GeometryPassMaterialFlags)ConsistentCharArrToInt32(a_arrIn);
    a_arrIn+=4;
    m_lightFlags = (RenderControl::LightTypeFlags)(*a_arrIn);
    a_arrIn+=1;
    unsigned int l_size = ConsistentCharArrToInt32(a_arrIn);
    a_arrIn+=4;
    m_meshPath = std::string(a_arrIn, l_size);
  }
  
  size_t ObjAddInfo::Size() const
  {
    return sizeof(uint32_t) + sizeof(ObjectType) + sizeof(RenderControl::GeometryPassMaterialFlags) + sizeof(RenderControl::LightTypeFlags) + sizeof(uint32_t) + m_meshPath.size();
  }
  
  void ObjTransformInfo::Serialize(char* a_arrOut) const
  {
    ConsistentInt32ToCharArray(m_id, a_arrOut);
    a_arrOut+=4;
    memcpy(a_arrOut, (void*)&m_transformType, 1);
    a_arrOut+=1;
    
    ConsistentFloatToCharArray(x, a_arrOut);
    a_arrOut+=4;
    ConsistentFloatToCharArray(y, a_arrOut);
    a_arrOut+=4;
    ConsistentFloatToCharArray(z, a_arrOut);
    a_arrOut+=4;
    
  }
  void ObjTransformInfo::Deserialize(char* a_arrIn)
  {
    m_id = ConsistentCharArrToInt32(a_arrIn);
    a_arrIn+=4;
    m_transformType = (ObjectTransformType)(*a_arrIn);
    a_arrIn+=1;
    
    x = ConsistentCharArrToFloat(a_arrIn);
    a_arrIn+=4;
    y = ConsistentCharArrToFloat(a_arrIn);
    a_arrIn+=4;
    z = ConsistentCharArrToFloat(a_arrIn);
    a_arrIn+=4;
  }
  
  size_t ObjTransformInfo::Size() const
  {
    return sizeof(uint32_t) + sizeof(ObjectTransformType) + 3* sizeof(float) ;
  }
  
  

  
  
  
  
  /*
   *  Method: NetworkMsg::NetworkMsg
   *  Params: 
   * Effects: 
   */
  NetworkMsg::NetworkMsg()
  :m_data(nullptr), m_size(0), m_trueSize(0), m_type(MsgType::NONE)
  {
  }


  /*
   *  Method: NetworkMsg::~NetworkMsg
   *  Params: 
   * Effects: 
   */
  NetworkMsg::~NetworkMsg()
  {
    Clear();
  }

  NetworkMsg::NetworkMsg(const NetworkMsg& a_other)
    :m_data(nullptr), m_size(a_other.m_size), m_trueSize(a_other.m_trueSize), m_type(a_other.m_type)
  {
    m_data = new char[m_trueSize];
    memcpy(m_data, a_other.m_data, a_other.m_size);
    
  }
  
  NetworkMsg::NetworkMsg(NetworkMsg&& a_other)
    :m_data( std::move(a_other.m_data) ), m_size( std::move(a_other.m_size) ), m_trueSize( std::move(a_other.m_trueSize) ), m_type( std::move(a_other.m_type) )
  {
    a_other.m_data = nullptr;
    a_other.m_size = 0;
    a_other.m_trueSize = 0;
    a_other.m_type = MsgType::NONE;
  }
  NetworkMsg& NetworkMsg::operator=(const NetworkMsg& a_other)
  {
    if( this != &a_other)
    {
      Reset(a_other.m_size);
      memcpy(m_data, a_other.m_data, a_other.m_size);
      
      m_size = a_other.m_size;
      m_trueSize = a_other.m_trueSize;
      m_type = a_other.m_type;
    }
    return *this;
  }
  NetworkMsg& NetworkMsg::operator=(NetworkMsg&& a_other)
  {
    if( this != &a_other)
    {
      Clear();
      m_data = std::move(a_other.m_data);
      m_size = std::move(a_other.m_size);
      m_trueSize = std::move(a_other.m_trueSize);
      m_type = std::move(a_other.m_type);
      
      a_other.m_data = nullptr;
      a_other.m_size = 0;
      a_other.m_trueSize = 0;
      a_other.m_type = MsgType::NONE;
    }
    return *this;
  }
  
  
  /*
   *  Method: NetworkMsg::CreateSizeMsg
   *  Params: 
   * Returns: void
   * Effects: 
   */
  void NetworkMsg::CreateSizeMsg(const uint32_t& a_size)
  {
    uint32_t l_size = 5; /// 1 char type + 1(32bit) number == 5 bytes
    Reset(l_size);
    m_type = MsgType::MSG_SIZE;
    m_size = l_size;
    char* l_pos = m_data;
    *l_pos = (char)m_type;
    
    ++l_pos;
    ConsistentInt32ToCharArray(a_size, l_pos);
  } 

  /*
   *  Method: NetworkMsg::CreateClientRequestMsg
   *  Params: 
   * Returns: void
   * Effects: 
   */
  void NetworkMsg::CreateClientRequestMsg()
  {
    uint32_t l_size = 1; /// 1 char type
    Reset(l_size);
    m_type = MsgType::CLNT_REQUEST;
    m_size = l_size;
    *m_data = (char)m_type;
  }

  /*
   *  Method: NetworkMsg::CreateEngineReadyMsg
   *  Params: 
   * Returns: void
   * Effects: 
   */
  void NetworkMsg::CreateEngineReadyMsg()
  {
    uint32_t l_size = 1; /// 1 char type
    Reset(l_size);
    m_type = MsgType::CLNT_ENGINE_READY;
    m_size = l_size;
    *m_data = (char)m_type;
  }

  /*
   *  Method: NetworkMsg::CreateGeometryPassMsg
   *  Params: 
   * Returns: void
   * Effects: 
   */
  void NetworkMsg::CreateRenderResultMsg(char* a_textureData, const uint32_t& a_textureDataSize, const glm::vec2& a_resolution)
  {
    uint32_t l_size = 13 + a_textureDataSize; /// 1 char type + 4(32bit) a_textureDataSize + 8(64bit) resolution + a_textureData == (5 + a_textureDataSize) bytes
    Reset(l_size);
    m_type = MsgType::CLNT_RENDER_RESULT;
    m_size = l_size;
    char* l_pos = m_data;
    *l_pos = (char)m_type;
    ++l_pos;
    
    ConsistentInt32ToCharArray(a_textureDataSize, l_pos);
    l_pos += 4;

    ConsistentInt32ToCharArray((uint32_t)a_resolution.x, l_pos);
    l_pos += 4;

    ConsistentInt32ToCharArray((uint32_t)a_resolution.y, l_pos);
    l_pos += 4;

    memcpy(l_pos, a_textureData, a_textureDataSize);
  }

  /*
   *  Method: NetworkMsg::CreateSetupMsg
   *  Params: 
   * Returns: void
   * Effects: 
   */
  void NetworkMsg::CreateSetupMsg( const glm::vec4& a_viewportInfo, const glm::vec2& a_partialResolution, const glm::vec2& a_resolution  )
  {
    uint32_t l_size = 33; /// 1 char type + 16(4*32bits) a_viewportInfo + 8(2*32bits) a_partialResolution + 8(2*32bits) a_resolution, 33 bytes
    Reset(l_size);
    m_type = MsgType::SRV_SETUP;
    m_size = l_size; 
    char* l_pos = m_data;
    *l_pos = (char)m_type;
    ++l_pos;
        
    // a_viewportInfo
    ConsistentInt32ToCharArray( (uint32_t)a_viewportInfo.x, l_pos);
    l_pos += 4;
    ConsistentInt32ToCharArray( (uint32_t)a_viewportInfo.y, l_pos);
    
    l_pos += 4;
    ConsistentInt32ToCharArray( (uint32_t)a_viewportInfo.z, l_pos);
    
    l_pos += 4;
    ConsistentInt32ToCharArray( (uint32_t)a_viewportInfo.w, l_pos);
    
    l_pos += 4;
    // a_partialResolution
    ConsistentInt32ToCharArray( (uint32_t)a_partialResolution.x, l_pos);
    
    l_pos += 4;
    ConsistentInt32ToCharArray( (uint32_t)a_partialResolution.y, l_pos);
    
    l_pos += 4;
    
    // a_resolution
    ConsistentInt32ToCharArray( (uint32_t)a_resolution.x, l_pos);
    
    l_pos += 4;
    ConsistentInt32ToCharArray( (uint32_t)a_resolution.y, l_pos);
    

  }

  /*
   *  Method: NetworkMsg::CreateSceneUpdateMsg
   *  Params: 
   * Returns: MsgType
   * Effects: 
   */
  void NetworkMsg::CreateSceneUpdateMsg(
      const std::array<glm::vec3, 3>& a_cameraSettings,
      const std::vector<ObjAddInfo>& a_objsToAdd, 
      const std::vector<uint32_t>& a_objsToRemove, 
      const std::vector<ObjTransformInfo>& a_objsToTransform, 
      const std::vector<TextureChangeInfo>& a_textureChange,
      
      const std::vector<ObjAddInfo>& a_lightsToAdd,
      const std::vector<uint32_t>& a_lightsToRemove,
      const std::vector<ObjTransformInfo>& a_lightsToTransform
      )
  {
    uint32_t l_size = 1 + 
      3 * 3 * sizeof(uint32_t) // camera settings
      + sizeof(uint32_t);
    for(unsigned int i = 0; i < a_objsToAdd.size(); ++i)
    {
      l_size += sizeof(uint32_t);
      l_size += a_objsToAdd[i].Size();
    }
    l_size += sizeof(uint32_t) + a_objsToRemove.size() * sizeof(uint32_t) +
              sizeof(uint32_t) + a_objsToTransform.size() * sizeof(ObjTransformInfo) +
              sizeof(uint32_t);
    for(unsigned int i = 0; i < a_textureChange.size(); ++i)
    {
      l_size += sizeof(uint32_t); // m_id
      l_size += sizeof(uint32_t); // m_textureLayer
      l_size += 1; // m_cubeText
      for( unsigned l_textIndex = 0; l_textIndex < 6; ++l_textIndex)
      {
        l_size += sizeof(uint32_t); // int size( m_path.length )
        l_size += a_textureChange[i].m_path[i].length();
      }
    }
    
    l_size += sizeof(uint32_t);
    for(unsigned int i = 0; i < a_lightsToAdd.size(); ++i)
    {
      l_size += sizeof(uint32_t);
      l_size += a_lightsToAdd[i].Size();
    }
    l_size += sizeof(uint32_t) + a_lightsToRemove.size() * sizeof(uint32_t) +
              sizeof(uint32_t) + a_lightsToTransform.size() * sizeof(ObjTransformInfo);
    
    
    Reset(l_size);
    m_type = MsgType::SRV_SCENE_UPDATE;
    m_size = l_size;
    char* l_pos = m_data;
    *l_pos = (char)m_type;
    ++l_pos;
    
    // camera settings
    for( unsigned int i = 0; i < 3; ++i)
    {
      for( unsigned int j = 0; j < 3; ++j)
      {
        ConsistentFloatToCharArray( a_cameraSettings[i][j], l_pos);
        l_pos += 4;
      }
    }
    
    
    // a_objsToAdd
    ConsistentInt32ToCharArray( uint32_t(a_objsToAdd.size()), l_pos);
    l_pos += 4;
    for( unsigned int i = 0; i < a_objsToAdd.size(); ++i)
    {
      ConsistentInt32ToCharArray( uint32_t(a_objsToAdd[i].Size() ) , l_pos);
      l_pos += 4;
      a_objsToAdd[i].Serialize(l_pos);
      l_pos += a_objsToAdd[i].Size();
    }
    
    // a_objsToRemove
    ConsistentInt32ToCharArray( uint32_t(a_objsToRemove.size() ), l_pos);
    l_pos += 4;
    for( unsigned int i = 0; i < a_objsToRemove.size(); ++i)
    {
      ConsistentInt32ToCharArray(a_objsToRemove[i], l_pos);
      l_pos += sizeof(uint32_t);      
    }
    
    // a_objsToTransform
    ConsistentInt32ToCharArray(uint32_t(a_objsToTransform.size()), l_pos);
    l_pos += 4;
    for( unsigned int i = 0; i < a_objsToTransform.size(); ++i)
    {
      a_objsToTransform[i].Serialize(l_pos);
      l_pos += a_objsToTransform[i].Size();      
    }
    
    
    // a_textureChange
    ConsistentInt32ToCharArray( uint32_t(a_textureChange.size()), l_pos);
    l_pos += 4;
    for(unsigned int i = 0; i < a_textureChange.size(); ++i)
    {
      ConsistentInt32ToCharArray(a_textureChange[i].m_id, l_pos);
      l_pos += 4;
      ConsistentInt32ToCharArray(a_textureChange[i].m_textureLayer, l_pos);
      l_pos += 4;
      *l_pos = a_textureChange[i].m_cubeText;
      l_pos += 1;
      for( unsigned l_textIndex = 0; l_textIndex < 6; ++l_textIndex)
      {
        ConsistentInt32ToCharArray( uint32_t(a_textureChange[i].m_path[l_textIndex].length() ), l_pos);
        l_pos += 4;
        memcpy(l_pos, &(a_textureChange[i].m_path[l_textIndex][0]), uint32_t(a_textureChange[i].m_path[l_textIndex].length() ) );
        l_pos += a_textureChange[i].m_path[l_textIndex].length();
      }
    }
    
    // a_lightsToAdd
    ConsistentInt32ToCharArray( uint32_t(a_lightsToAdd.size() ), l_pos);
    l_pos += 4;
    for( unsigned int i = 0; i < a_lightsToAdd.size(); ++i)
    {
      ConsistentInt32ToCharArray( uint32_t(a_lightsToAdd[i].Size()), l_pos);
      l_pos += 4;
      a_lightsToAdd[i].Serialize(l_pos);
      l_pos += a_lightsToAdd[i].Size();
    }
    
    // a_lightsToRemove
    ConsistentInt32ToCharArray(uint32_t(a_lightsToRemove.size()), l_pos);
    l_pos += 4;
    for( unsigned int i = 0; i < a_lightsToRemove.size(); ++i)
    {
      ConsistentInt32ToCharArray(a_lightsToRemove[i], l_pos);
      l_pos += sizeof(uint32_t);      
    }
    
    // a_lightsToTransform
    ConsistentInt32ToCharArray( uint32_t(a_lightsToTransform.size() ), l_pos);
    l_pos += 4;
    for( unsigned int i = 0; i < a_lightsToTransform.size(); ++i)
    {
      a_lightsToTransform[i].Serialize(l_pos);
      l_pos += sizeof(ObjTransformInfo);      
    }
    
  }

  

  /*
   *  Method: NetworkMsg::GetType
   *  Params: 
   * Returns: MsgType
   * Effects: 
   */
  MsgType NetworkMsg::GetType() const
  {
    if( m_size < 1 )
      return MsgType::NONE;
    return (MsgType)m_data[0];
  }
  
  
  /*
   *  Method: NetworkMsg::DeserializeSizeMsg
   *  Params: 
   * Returns: bool
   * Effects: 
   */
  bool NetworkMsg::DeserializeSizeMsg(uint32_t& a_outSize) const 
  {
    
    if( (MsgType)m_data[0] != MsgType::MSG_SIZE || m_size != 5)
      return false;
      
    // a_outSize = ((uint32_t)m_data[4] << 24) | ((uint32_t)m_data[3] << 16) | ((uint32_t)m_data[2] << 8) | (uint32_t)m_data[1];
    a_outSize = ConsistentCharArrToInt32(&m_data[1]);
    return true;
  }

  /*
   *  Method: NetworkMsg::DeserializeGeometryPassMsg
   *  Params: 
   * Returns: bool
   * Effects: 
   */
  bool NetworkMsg::DeserializeRenderResultMsg( char* a_outTextureData, uint32_t& a_outTextureDataSize, glm::vec2& a_outResolution) const 
  {
    char* l_pos = m_data;
    if(m_size < 5)
      return false;
    if( (MsgType)m_data[0] != MsgType::CLNT_RENDER_RESULT )
      return false;
    
    ++l_pos;

    
    a_outTextureDataSize = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    a_outResolution.x = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    a_outResolution.y = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    if( a_outTextureData )
      memcpy(a_outTextureData, l_pos, a_outTextureDataSize);
    return true;
  }

    /*
   *  Method: NetworkMsg::DeserializeRenderResultMsgWithoutCopy
   *  Params: 
   * Returns: bool
   * Effects: 
   */
  bool NetworkMsg::DeserializeRenderResultMsgWithoutCopy( char*& a_outTextureData, uint32_t& a_outTextureDataSize, glm::vec2& a_outResolution) const 
  {
    char* l_pos = m_data;
    if(m_size < 5)
      return false;
    if( (MsgType)m_data[0] != MsgType::CLNT_RENDER_RESULT )
      return false;
    
    ++l_pos;

    
    a_outTextureDataSize = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    a_outResolution.x = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    a_outResolution.y = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;

    // NO COPY HERE
    a_outTextureData = l_pos;
    return true;
  }
  
  /*
   *  Method: NetworkMsg::DeserializeSetupMsg
   *  Params: 
   * Returns: bool
   * Effects: 
   */
  bool NetworkMsg::DeserializeSetupMsg( glm::vec4& a_outViewportInfo, glm::vec2& a_outPartialResolution, glm::vec2& a_outResolution   ) const 
  {
    char* l_pos = m_data;
    if(m_size < 33)
      return false;
    if( (MsgType)(*l_pos) != MsgType::SRV_SETUP )
      return false;

    uint32_t l_x, l_y, l_z, l_w;
    
    ++l_pos;
 
    // a_outViewportInfo
    l_x = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    l_y = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    l_z = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    l_w = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    a_outViewportInfo = glm::vec4(l_x, l_y, l_z, l_w);
    
    // a_outPartialResolution
    l_x = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    l_y = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    a_outPartialResolution = glm::vec2(l_x, l_y);
    
    // a_outResolution
    l_x = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    l_y = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    a_outResolution = glm::vec2(l_x, l_y);
    
    return true;
  }

  /*
   *  Method: NetworkMsg::DeserializeSceneUpdateMsg
   *  Params: 
   * Returns: bool
   * Effects: 
   */
  bool NetworkMsg::DeserializeSceneUpdateMsg(
      std::array<glm::vec3, 3>& a_outCameraSettings,
      
      std::vector<ObjAddInfo>& a_outObjsToAdd, 
      std::vector<uint32_t>& a_outObjsToRemove, 
      std::vector<ObjTransformInfo>& a_outObjsToTransform, 
      std::vector<TextureChangeInfo>& a_outTextureChange,
      
      std::vector<ObjAddInfo>& a_outLightsToAdd, 
      std::vector<uint32_t>& a_outLightsToRemove, 
      std::vector<ObjTransformInfo>& a_outLightsToTransform
      ) const
  {
    char* l_pos = m_data;
    if(m_size < 29)
      return false;
    if( (MsgType)(*l_pos) != MsgType::SRV_SCENE_UPDATE )
      return false;
    ++l_pos;
    
    // camera settings
    for( unsigned int i = 0; i < 3; ++i)
    {
      a_outCameraSettings[i] = glm::vec3(ConsistentCharArrToFloat(l_pos), ConsistentCharArrToFloat(l_pos+sizeof(float)), ConsistentCharArrToFloat(l_pos+2*sizeof(float)) );
      l_pos += 3*sizeof(float);
    }
    // a_outObjsToAdd
    uint32_t l_objectsToAddSize = ConsistentCharArrToInt32(l_pos);
    a_outObjsToAdd.resize(l_objectsToAddSize);
    l_pos += 4;
    for( unsigned int i = 0; i < l_objectsToAddSize; ++i)
    {
      uint32_t l_objAddInfoSize = ConsistentCharArrToInt32(l_pos);
      l_pos += 4;
      a_outObjsToAdd[i].Deserialize(l_pos);
      l_pos += l_objAddInfoSize;
      
    }
    
    // a_outObjsToRemove
    uint32_t l_outObjsToRemoveSize = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;
    a_outObjsToRemove.resize(l_outObjsToRemoveSize);
    if( l_outObjsToRemoveSize > 0)
    {
      memcpy( &a_outObjsToRemove[0], l_pos, l_outObjsToRemoveSize*sizeof(uint32_t) );
      l_pos += l_outObjsToRemoveSize*sizeof(uint32_t);
    }
      
    // a_outObjsToTransform
    uint32_t l_outObjsToTransformSize = ConsistentCharArrToInt32(l_pos);
    l_pos += 4;  
    a_outObjsToTransform.resize(l_outObjsToTransformSize);
    if(l_outObjsToTransformSize > 0)
    {
      memcpy( &a_outObjsToTransform[0], l_pos, l_outObjsToTransformSize*sizeof(ObjTransformInfo) );
      l_pos += l_outObjsToTransformSize*sizeof(ObjTransformInfo);
    }
    
    // a_outTextureChange
    uint32_t l_outTextureChangeSize = ConsistentCharArrToInt32(l_pos);
    a_outTextureChange.resize(l_outTextureChangeSize);
    l_pos += 4;
    for( unsigned int i = 0; i < l_outTextureChangeSize; ++i)
    {
      a_outTextureChange[i].m_id = ConsistentCharArrToInt32(l_pos);
      l_pos += 4;
      a_outTextureChange[i].m_textureLayer = ConsistentCharArrToInt32(l_pos);
      l_pos += 4;
      a_outTextureChange[i].m_cubeText = *l_pos;
      l_pos += 1;
      for( unsigned l_textIndex = 0; l_textIndex < 6; ++l_textIndex)
      {
        unsigned int l_textSize = ConsistentCharArrToInt32(l_pos);
        l_pos += 4;
        a_outTextureChange[i].m_path[l_textIndex] = std::string(l_pos, l_textSize);
        l_pos += l_textSize;
      }
    }
    
    
    // a_outLightsToAdd
    uint32_t l_outLightsToAddSize = ConsistentCharArrToInt32(l_pos);
    a_outLightsToAdd.resize(l_outLightsToAddSize);
    l_pos += 4;
    for( unsigned int i = 0; i < l_outLightsToAddSize; ++i)
    {
      uint32_t l_objAddInfoSize = ConsistentCharArrToInt32(l_pos);
      l_pos += 4;
      a_outLightsToAdd[i].Deserialize(l_pos);
      l_pos += l_objAddInfoSize;
    }
    
    // a_outLightsToRemove
    uint32_t l_outLightsToRemoveSize = ConsistentCharArrToInt32(l_pos);
    a_outLightsToRemove.resize(l_outLightsToRemoveSize);
    l_pos += 4;
    for( unsigned int i = 0; i < l_outLightsToRemoveSize; ++i)
    {
      a_outLightsToRemove[i] = ConsistentCharArrToInt32(l_pos);
      l_pos += sizeof(uint32_t);
    }
    
    // a_outLightsToTransform
    uint32_t l_outLightsToTransformSize = ConsistentCharArrToInt32(l_pos);
    a_outLightsToTransform.resize(l_outLightsToTransformSize);
    l_pos += 4;
    for( unsigned int i = 0; i < l_outLightsToTransformSize; ++i)
    {
      a_outLightsToTransform[i].Deserialize(l_pos);
      l_pos += sizeof(ObjTransformInfo);
    }
    
    return true;
  }

  /*
   *  Method: NetworkMsg::Reset
   *  Params: 
   * Returns: void
   * Effects: 
   */
  void NetworkMsg::Reset(const uint32_t& a_sizeTrue)
  {
    if( m_data && a_sizeTrue > m_trueSize)
    {
      Clear();
      m_data = new char[a_sizeTrue];
      m_type = MsgType::NONE;
      m_size = 0;
      m_trueSize = a_sizeTrue; 
    }
    else if( m_data && a_sizeTrue <= m_trueSize )
    {
      m_type = MsgType::NONE;
      m_size = 0;
    }
    else if( !m_data )
    {
      m_data = new char[a_sizeTrue];
      m_type = MsgType::NONE;
      m_size = 0;
      m_trueSize = a_sizeTrue; 
    }
  }

  /*
   *  Method: NetworkMsg::Clear
   *  Params: 
   * Returns: void
   * Effects: 
   */
  void NetworkMsg::Clear()
  {
    if( m_data )
    {
      delete [] m_data;
      m_data = nullptr;
    }

    m_type = MsgType::NONE;
    m_size = 0;
    m_trueSize = 0; 
  }
  
  std::ostream& operator<<(std::ostream &strm, const NetworkMsg &a)
  {
    if( a.m_size > 0)
    {
      switch(a.m_data[0])
      {
        case MsgType::MSG_SIZE: 
          uint32_t a_outSize;
          a.DeserializeSizeMsg(a_outSize);
          strm << "MSG_SIZE"; 
          strm << a_outSize;
          break;
        case MsgType::CLNT_REQUEST: strm << "CLNT_REQUEST"; break;
        case MsgType::CLNT_ENGINE_READY: strm << "CLNT_ENGINE_READY"; break;
        case MsgType::CLNT_RENDER_RESULT: strm << "CLNT_RENDER_RESULT"; return strm;
        case MsgType::SRV_SETUP:
        {
          strm << "SRV_SETUP: " << std::endl; 
          glm::vec4 l_viewport;
          glm::vec2 l_resPart;
          glm::vec2 l_res;
          a.DeserializeSetupMsg(l_viewport, l_resPart, l_res);
          
          strm << "Viewport settings " << (uint32_t)l_viewport.x << ", " << (uint32_t)l_viewport.y << ", " << (uint32_t)l_viewport.z << ", " << (uint32_t)l_viewport.w << std::endl;
          strm << "partial resolution " << (uint32_t)l_resPart.x << ", " << (uint32_t)l_resPart.y << std::endl;
          strm << "resolution " << (uint32_t) l_res.x << ", " << (uint32_t)l_res.y;
          return strm;
        }
        case MsgType::SRV_SCENE_UPDATE: 
        {
          strm << "SRV_SCENE_UPDATE:" << std::endl;
          std::array<glm::vec3, 3> l_outCameraSettings;
          std::vector<Network::ObjAddInfo> l_outObjsToAdd; 
          std::vector<uint32_t> l_outObjsToRemove;
          std::vector<Network::ObjTransformInfo> l_outObjsToTransform; 
          std::vector<Network::TextureChangeInfo> l_outTextureChange;
          std::vector<Network::ObjAddInfo> l_outLightsToAdd; 
          std::vector<uint32_t> l_outLightsToRemove;
          std::vector<Network::ObjTransformInfo> l_outLightsToTransform;
          
          a.DeserializeSceneUpdateMsg(l_outCameraSettings, l_outObjsToAdd, l_outObjsToRemove, l_outObjsToTransform, l_outTextureChange, l_outLightsToAdd, l_outLightsToRemove, l_outLightsToTransform);
          strm << "Camera Settings:" << std::endl;
          for( unsigned int i = 0; i< 3; ++i)
          {
            strm << "setting[" <<  i << "] :" << l_outCameraSettings[i][0] << ", " << l_outCameraSettings[i][1] << ", " << l_outCameraSettings[i][2] << ", " << std::endl;
          }
          
          if(l_outObjsToAdd.size() ) strm << "\tObjects to add:" << std::endl;
          for( unsigned int i = 0; i < l_outObjsToAdd.size(); ++i)
          {
            strm << l_outObjsToAdd[i] << std::endl;
          }
          if(l_outObjsToRemove.size() ) strm << "\tObjects to remove:" << std::endl;
          for( unsigned int i = 0; i < l_outObjsToRemove.size(); ++i)
          {
            strm << "id:" << l_outObjsToRemove[i] << std::endl;
          }
          if(l_outObjsToTransform.size() ) strm << "\tObjects to transform:" << std::endl;
          for( unsigned int i = 0; i < l_outObjsToTransform.size(); ++i)
          {
            strm << "id:" << l_outObjsToTransform[i].m_id << std::endl;
            strm << "trans type:" << (uint32_t)l_outObjsToTransform[i].m_transformType << std::endl;
            strm << "vec3:" << l_outObjsToTransform[i].x << " " << l_outObjsToTransform[i].y << " " << l_outObjsToTransform[i].z << std::endl;
          }
          if(l_outTextureChange.size() ) strm << "\tTexture change:" << std::endl;
          for( unsigned int i = 0; i < l_outTextureChange.size(); ++i)
          {
            strm << "id:" << l_outTextureChange[i].m_id << std::endl;
            strm << "texture layer :" << l_outTextureChange[i].m_textureLayer << std::endl;
            
            unsigned int l_s = l_outTextureChange[i].m_cubeText == 1 ? 6 : 1;
            for( unsigned j =0; j < l_s; ++j)
              strm << "\tm_path[" << j << "] : " << l_outTextureChange[i].m_path[j] << std::endl;
          }
          return strm;
        }
        default : strm << "NONE - ";
      }
    }  
    else 
    {
      strm << "NONE" << std::endl;
      return strm;
    }
    if( a.m_size > 1)
      strm << std::string(&a.m_data[1], a.m_size-1) << std::endl;
    return strm;
  }
  
  std::ostream& operator<<(std::ostream &strm, const ObjAddInfo &a)
  {
    strm << "id:" << a.m_id << std::endl;
    strm << "Object Type:";
    switch( a.m_objType )
    {
      case Network::ObjectType::CUBE: 
      strm << "CUBE"<< std::endl;
      break;
    case Network::ObjectType::SKYBOX: 
      strm << "SKYBOX"<< std::endl;
      break;
    case Network::ObjectType::SPHERE: 
      strm << "SPHERE"<< std::endl;
      break;
    case Network::ObjectType::MESH:
      strm << "MESH"<< std::endl;
      break;
    }
    strm << "material flags:" << (uint32_t)a.m_materialFlags << std::endl;
    strm << "mesh path:" << a.m_meshPath << std::endl;
    return  strm;
  }
  
}