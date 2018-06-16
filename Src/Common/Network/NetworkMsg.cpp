/***********************************************************************
 * AUTHOR:  <Doublecross>
 *   FILE: NetworkMsg.cpp
 *   DATE: Fri Jun 15 13:26:36 2018
 *  DESCR: 
 ***********************************************************************/
#include "Common/Network/NetworkMsg.h"
namespace Network
{
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
    memcpy(l_pos, &a_size, 4);
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
    m_type = MsgType::CLNT_ENGINE_READY;
    m_size = l_size;
    *m_data = (char)m_type;
  }

  /*
   *  Method: NetworkMsg::CreateEngineReadyMsg
   *  Params: 
   * Returns: void
   * Effects: 
   */
  void NetworkMsg::CreateEngineReadyMsg(const uint32_t& a_clientId)
  {
    uint32_t l_size = 5; /// 1 char type + 1(32bit) number == 5 bytes
    Reset(l_size);
    m_type = MsgType::CLNT_ENGINE_READY;
    m_size = l_size;
    char* l_pos = m_data;
    *l_pos = (char)m_type;
    
    ++l_pos;
    memcpy(l_pos, &a_clientId, 4);
  }

  /*
   *  Method: NetworkMsg::CreateGeometryPassMsg
   *  Params: 
   * Returns: void
   * Effects: 
   */
  void NetworkMsg::CreateGeometryPassMsg(const uint32_t& a_clientId, char* a_textureData, const uint32_t& a_textureDataSize)
  {
    uint32_t l_size = 9 + a_textureDataSize; /// 1 char type + 1(32bit) a_clientId + 1(32bit) a_textureDataSize + a_textureData == (9 + a_textureDataSize) bytes
    Reset(l_size);
    m_type = MsgType::CLNT_GEOMETRY_PASS;
    m_size = l_size; /// 1 char type + 1(32bit) a_clientId + 1(32bit) a_textureDataSize + a_textureData == (9 + a_textureDataSize) bytes
    char* l_pos = m_data;
    *l_pos = (char)m_type;
    ++l_pos;
    
    memcpy(l_pos, &a_clientId, 4);
    l_pos += 4;

    memcpy(l_pos, &a_textureDataSize, 4);
    l_pos += 4;

    memcpy(l_pos, a_textureData, a_textureDataSize);
  }

  /*
   *  Method: NetworkMsg::CreateLightPassMsg
   *  Params: 
   * Returns: void
   * Effects: 
   */
  void NetworkMsg::CreateLightPassMsg(const uint32_t& a_clientId, char* a_textureData, const uint32_t& a_textureDataSize)
  {
    uint32_t l_size = 9 + a_textureDataSize; /// 1 char type + 1(32bit) a_clientId + 1(32bit) a_textureDataSize + a_textureData == (9 + a_textureDataSize) bytes
    Reset(l_size);
    m_type = MsgType::CLNT_LIGHT_PASS;
    m_size = l_size;

    char* l_pos = m_data;
    *l_pos = (char)m_type;
    ++l_pos;
    
    memcpy(l_pos, &a_clientId, 4);
    l_pos += 4;

    memcpy(l_pos, &a_textureDataSize, 4);
    l_pos += 4;

    memcpy(l_pos, a_textureData, a_textureDataSize);
  }

  /*
   *  Method: NetworkMsg::CreateSetupMsg
   *  Params: 
   * Returns: void
   * Effects: 
   */
  void NetworkMsg::CreateSetupMsg(const uint32_t& a_clientId, const glm::vec2& a_geometryPassTexSize, const glm::vec4& a_viewportInfo, const glm::vec2& a_lightPassTexSize )
  {
    uint32_t l_size = 33; /// 1 char type + 8(2*32bits) a_geometryPassTextSize, 16(4*32bits) a_viewportInfo + 8(2*32bits) a_lightPassTexSize, 33 bytes
    Reset(l_size);
    m_type = MsgType::SRV_SETUP;
    m_size = l_size; /// 1 char type + 8(2*32bits) a_geometryPassTextSize, 16(4*32bits) a_viewportInfo + 8(2*32bits) a_lightPassTexSize, 33 bytes
    char* l_pos = m_data;
    *l_pos = (char)m_type;
    ++l_pos;
    
    memcpy(l_pos, &a_clientId, 4);
    l_pos += 4;
    
    // a_geometryPassTexSize
    memcpy(l_pos, &a_geometryPassTexSize.x, 4);
    l_pos += 4;
    
    memcpy(l_pos, &a_geometryPassTexSize.y, 4);
    l_pos += 4;
    
    // a_viewportInfo
    memcpy(l_pos, &a_viewportInfo.x, 4);
    l_pos += 4;
    memcpy(l_pos, &a_viewportInfo.y, 4);
    l_pos += 4;
    memcpy(l_pos, &a_viewportInfo.z, 4);
    l_pos += 4;
    memcpy(l_pos, &a_viewportInfo.w, 4);
    l_pos += 4;
    
    // a_lightPassTexSize
    memcpy(l_pos, &a_lightPassTexSize.x, 4);
    l_pos += 4;
    memcpy(l_pos, &a_lightPassTexSize.y, 4);
  }

  /*
   *  Method: NetworkMsg::CreateSceneUpdateMsg
   *  Params: 
   * Returns: MsgType
   * Effects: 
   */
  void NetworkMsg::CreateSceneUpdateMsg(const std::vector<ObjAddInfo>& a_objsToAdd, const std::vector<uint32_t>& a_objsToRemove, const std::vector<ObjTransformInfo>& a_objsToTransform, const std::vector<TextureChangeInfo>& a_textureChange)
  {
    uint32_t l_size = 1 + 
                      sizeof(uint32_t) + a_objsToAdd.size() * sizeof(ObjAddInfo) + 
                      sizeof(uint32_t) + a_objsToRemove.size() * sizeof(uint32_t) +
                      sizeof(uint32_t) + a_objsToTransform.size() * sizeof(ObjTransformInfo) +
                      sizeof(uint32_t);
    for(unsigned int i = 0; i < a_textureChange.size(); ++i)
    {
      l_size += sizeof(uint32_t); // m_id
      l_size += sizeof(uint32_t); // m_textureLayer
      l_size += sizeof(uint32_t); // int size( m_path.length )
      l_size += a_textureChange[i].m_path.length();
    }
    
    Reset(l_size);
    m_type = MsgType::SRV_SCENE_UPDATE;
    m_size = l_size;
    char* l_pos = m_data;
    *l_pos = (char)m_type;
    ++l_pos;
    
    
    // a_objsToAdd
    uint32_t l_tmp = a_objsToAdd.size();
    memcpy(l_pos, &l_tmp, sizeof(uint32_t) );
    l_pos += 4;
    memcpy(l_pos, &a_objsToAdd[0], a_objsToAdd.size() * sizeof(ObjAddInfo) );
    l_pos += a_objsToAdd.size() * sizeof(ObjAddInfo);
    
    // a_objsToRemove
    l_tmp = a_objsToRemove.size();
    memcpy(l_pos, &l_tmp, sizeof(uint32_t) );
    l_pos += 4;
    memcpy(l_pos, &a_objsToRemove[0], a_objsToRemove.size() * sizeof(uint32_t) );
    l_pos += a_objsToRemove.size() * sizeof(uint32_t);
    
    // a_objsToTransform
    l_tmp = a_objsToTransform.size();
    memcpy(l_pos, &l_tmp, sizeof(uint32_t) );
    l_pos += 4;
    memcpy(l_pos, &a_objsToTransform[0], a_objsToTransform.size() * sizeof(ObjTransformInfo) );
    l_pos += a_objsToTransform.size() * sizeof(ObjTransformInfo);
    
    // a_textureChange
    l_tmp = a_textureChange.size();
    memcpy(l_pos, &l_tmp, sizeof(uint32_t) );
    l_pos += 4;
    for(unsigned int i = 0; i < a_textureChange.size(); ++i)
    {
      memcpy(l_pos, &(a_textureChange[i].m_id), 4);
      l_pos += 4;
      memcpy(l_pos, &(a_textureChange[i].m_textureLayer), 4);
      l_pos += 4;
      l_tmp = a_textureChange[i].m_path.length();
      memcpy(l_pos, &l_tmp, 4);
      l_pos += 4;
      memcpy(l_pos, &(a_textureChange[i].m_path[0]), a_textureChange[i].m_path.length());
      l_pos += a_textureChange[i].m_path.length();
    }
  }

  /*
   *  Method: NetworkMsg::CreateGeometryPassPlusLightsMsg
   *  Params: 
   * Returns: MsgType
   * Effects: 
   */
  void NetworkMsg::CreateGeometryPassPlusLightsMsg(char* a_textureData, const uint32_t& a_textureDataSize, const std::vector<ObjAddInfo>& a_lightsToAdd, const std::vector<uint32_t>& a_lightsToRemove, const std::vector<ObjTransformInfo>& a_lightsToTransform )
  {
    uint32_t l_size = 1 + 
                      sizeof(uint32_t) + a_textureDataSize +
                      sizeof(uint32_t) + a_lightsToAdd.size() + sizeof(ObjAddInfo) +
                      sizeof(uint32_t) + a_lightsToRemove.size() + sizeof(uint32_t) +
                      sizeof(uint32_t) + a_lightsToTransform.size() + sizeof(ObjTransformInfo);
    
    Reset(l_size);
    m_type = MsgType::SRV_GEOMETRY_PASS_PLUS_LIGHTS;
    m_size = l_size;
    char* l_pos = m_data;
    *l_pos = (char)m_type;
    ++l_pos;
    
    // texture data
    memcpy(l_pos, &a_textureDataSize, sizeof(uint32_t) );
    l_pos += 4;
    memcpy(l_pos, a_textureData, a_textureDataSize );
    l_pos += a_textureDataSize;
    
    // a_lightsToAdd
    uint32_t l_tmp = a_lightsToAdd.size();
    memcpy(l_pos, &l_tmp, sizeof(uint32_t) );
    l_pos += 4;
    memcpy(l_pos, &a_lightsToAdd[0], a_lightsToAdd.size() * sizeof(ObjAddInfo) );
    l_pos += a_lightsToAdd.size() * sizeof(ObjAddInfo);
    
    // a_lightsToRemove
    l_tmp = a_lightsToRemove.size();
    memcpy(l_pos, &l_tmp, sizeof(uint32_t) );
    l_pos += 4;
    memcpy(l_pos, &a_lightsToRemove[0], a_lightsToRemove.size() * sizeof(uint32_t) );
    l_pos += a_lightsToRemove.size() * sizeof(uint32_t);
    
    // a_lightsToTransform
    l_tmp = a_lightsToTransform.size();
    memcpy(l_pos, &l_tmp, sizeof(uint32_t) );
    l_pos += 4;
    memcpy(l_pos, &a_lightsToTransform[0], a_lightsToTransform.size() * sizeof(ObjTransformInfo) );

  }

  /*
   *  Method: NetworkMsg::GetType
   *  Params: 
   * Returns: MsgType
   * Effects: 
   */
  MsgType NetworkMsg::GetType(char* a_data, const uint32_t& a_dataSize) const
  {
    if( a_dataSize < 1 )
      return MsgType::NONE;
    return (MsgType)a_data[0];
  }
  /*
   *  Method: NetworkMsg::DeserializeSizeMsg
   *  Params: 
   * Returns: bool
   * Effects: 
   */
  bool NetworkMsg::DeserializeSizeMsg(char* a_data, const uint32_t& a_dataSize, uint32_t& a_outSize) const
  {
    
    if( (MsgType)a_data[0] != MsgType::MSG_SIZE || a_dataSize != 5)
      return false;
      
    a_outSize = ((uint32_t)a_data[1] << 24) | ((uint32_t)a_data[2] << 16) | ((uint32_t)a_data[3] << 8) | (uint32_t)a_data[4];
    return true;
  }

  /*
   *  Method: NetworkMsg::DeserializeEngineReadyMsg
   *  Params: 
   * Returns: bool
   * Effects: 
   */
  bool NetworkMsg::DeserializeEngineReadyMsg(char* a_data, const uint32_t& a_dataSize, uint32_t& a_outClientId) const
  {
    if( (MsgType)a_data[0] != MsgType::CLNT_ENGINE_READY || a_dataSize != 5)
      return false;
    
    a_outClientId = ((uint32_t)a_data[1] << 24) | ((uint32_t)a_data[2] << 16) | ((uint32_t)a_data[3] << 8) | (uint32_t)a_data[4];
    return true;
  }

  /*
   *  Method: NetworkMsg::DeserializeGeometryPassMsg
   *  Params: 
   * Returns: bool
   * Effects: 
   */
  bool NetworkMsg::DeserializeGeometryPassMsg(char* a_data, const uint32_t& a_dataSize, uint32_t& a_outClientId, char* a_outTextureData, uint32_t& a_outTextureDataSize) const
  {
    char* l_pos = m_data;
    if(a_dataSize < 9)
      return false;
    if( (MsgType)a_data[0] != MsgType::CLNT_GEOMETRY_PASS )
      return false;
    
    ++l_pos;
    a_outClientId = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    
    a_outTextureDataSize = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    
    memcpy(a_outTextureData, l_pos, a_outTextureDataSize);
    return true;
  }

  /*
   *  Method: NetworkMsg::DeserializeLightPassMsg
   *  Params: 
   * Returns: bool
   * Effects: 
   */
  bool NetworkMsg::DeserializeLightPassMsg(char* a_data, const uint32_t& a_dataSize, uint32_t& a_outClientId, char* a_outTextureData, uint32_t& a_outTextureDataSize) const
  {
    char* l_pos = a_data;
    if(a_dataSize < 9)
      return false;
    if( (MsgType)(*l_pos) != MsgType::CLNT_LIGHT_PASS )
      return false;
    
    ++l_pos;
    a_outClientId = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    
    a_outTextureDataSize = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    
    memcpy(a_outTextureData, l_pos, a_outTextureDataSize);
    return true;  
  }

  /*
   *  Method: NetworkMsg::DeserializeSetupMsg
   *  Params: 
   * Returns: bool
   * Effects: 
   */
  bool NetworkMsg::DeserializeSetupMsg(char* a_data, const uint32_t& a_dataSize, uint32_t& a_outClientId, glm::vec2& a_outGeometryPassTexSize, glm::vec4& a_outViewportInfo, glm::vec2& a_outLightPassTexSize) const
  {
    char* l_pos = a_data;
    if(a_dataSize < 33)
      return false;
    if( (MsgType)(*l_pos) != MsgType::SRV_SETUP )
      return false;

    uint32_t l_x, l_y, l_z, l_w;
    
    ++l_pos;
    // a_outClientId
    a_outClientId = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    
    // a_outGeometryPassTexSize
    l_x = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    l_y = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    a_outGeometryPassTexSize = glm::vec2(l_x, l_y);
    
    // a_outViewportInfo
    l_x = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    l_y = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    l_z = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    l_w = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    a_outViewportInfo = glm::vec4(l_x, l_y, l_z, l_w);
    
    // a_outLightPassTexSize
    l_x = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    l_y = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    a_outLightPassTexSize = glm::vec2(l_x, l_y);
    
    return true;
  }

  /*
   *  Method: NetworkMsg::DeserializeSceneUpdateMsg
   *  Params: 
   * Returns: bool
   * Effects: 
   */
  bool NetworkMsg::DeserializeSceneUpdateMsg(char* a_data, const uint32_t& a_dataSize, std::vector<ObjAddInfo>& a_outObjsToAdd, std::vector<uint32_t>& a_outObjsToRemove, std::vector<ObjTransformInfo>& a_outObjsToTransform, std::vector<TextureChangeInfo>& a_outTextureChange)
  {
    char* l_pos = a_data;
    if(a_dataSize < 33)
      return false;
    if( (MsgType)(*l_pos) != MsgType::SRV_SCENE_UPDATE )
      return false;
    ++l_pos;
    
    // a_outObjsToAdd
    uint32_t l_objectsToAddSize = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    a_outObjsToAdd.resize(l_objectsToAddSize);
    l_pos += 4;  
    memcpy( &a_outObjsToAdd[0], l_pos, l_objectsToAddSize*sizeof(ObjAddInfo) );
    l_pos += l_objectsToAddSize*sizeof(ObjAddInfo);
    
    // a_outObjsToRemove
    uint32_t l_outObjsToRemoveSize = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    a_outObjsToRemove.resize(l_outObjsToRemoveSize);
    l_pos += 4;
    memcpy( &a_outObjsToRemove[0], l_pos, l_outObjsToRemoveSize*sizeof(uint32_t) );
    l_pos += l_outObjsToRemoveSize*sizeof(uint32_t);

      
    // a_outObjsToTransform
    uint32_t l_outObjsToTransformSize = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    a_outObjsToTransform.resize(l_outObjsToTransformSize);
    l_pos += 4;  
    memcpy( &a_outObjsToTransform[0], l_pos, l_outObjsToTransformSize*sizeof(ObjTransformInfo) );
    l_pos += l_outObjsToTransformSize*sizeof(ObjTransformInfo);

    
    // a_outTextureChange
    uint32_t l_outTextureChangeSize = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    a_outTextureChange.resize(l_outTextureChangeSize);
    l_pos += 4;
    memcpy( &a_outTextureChange[0], l_pos, l_outTextureChangeSize*sizeof(TextureChangeInfo) );
    l_pos += l_outTextureChangeSize*sizeof(TextureChangeInfo);
    
    return true;
  }

  /*
   *  Method: NetworkMsg::DeserializeSceneUpdateMsg
   *  Params: 
   * Returns: bool
   * Effects: 
   */
  bool NetworkMsg::DeserializeGeometryPassPlusLightsMsg(char* a_data, const uint32_t& a_dataSize, char* a_outTextureData, uint32_t& a_outTextureDataSize, std::vector<ObjAddInfo>& a_outLightsToAdd, std::vector<uint32_t>& a_outLightsToRemove, std::vector<ObjTransformInfo>& a_outLightsToTransform )
  {
    char* l_pos = a_data;
    if(a_dataSize < 33)
      return false;
    if( (MsgType)(*l_pos) != MsgType::SRV_GEOMETRY_PASS_PLUS_LIGHTS )
      return false;
    ++l_pos;
    
    // a_outTextureData
    a_outTextureDataSize = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    l_pos += 4;
    memcpy( a_outTextureData, l_pos, a_outTextureDataSize );
    l_pos += a_outTextureDataSize;
    
    // a_outLightsToAdd
    uint32_t l_outLightsToAddSize = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    a_outLightsToAdd.resize(l_outLightsToAddSize);
    l_pos += 4;
    memcpy( &a_outLightsToAdd[0], l_pos, l_outLightsToAddSize * sizeof(ObjAddInfo) );
    l_pos += l_outLightsToAddSize *sizeof(ObjAddInfo);
    
    // a_outLightsToRemove
    uint32_t l_outLightsToRemoveSize = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    a_outLightsToRemove.resize(l_outLightsToRemoveSize);
    l_pos += 4;
    memcpy( &a_outLightsToRemove[0], l_pos, l_outLightsToRemoveSize * sizeof(uint32_t) );
    l_pos += l_outLightsToRemoveSize *sizeof(uint32_t);
    
    // a_outLightsToTransform
    uint32_t l_outLightsToTransformSize = ((uint32_t)(*l_pos) << 24) | ((uint32_t)(*l_pos+1) << 16) | ((uint32_t)(*l_pos+2) << 8) | (uint32_t)(*l_pos+3);
    a_outLightsToTransform.resize(l_outLightsToTransformSize);
    l_pos += 4;
    memcpy( &a_outLightsToTransform[0], l_pos, l_outLightsToTransformSize * sizeof(ObjTransformInfo) );
    l_pos += l_outLightsToTransformSize *sizeof(ObjTransformInfo);
    
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
  
}