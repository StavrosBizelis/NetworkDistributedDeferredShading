#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Common/RenderControl/ADeferredShadingPass.h"

namespace Network
{
    
  /// size of char is 1 byte - we dont need more
  enum class MsgType : char 
  {
    NONE,
    /// MESSAGE THAT IS SENT TO INFORM THE RECEIVER ABOUT THE NEXT MESSAGE SIZE - IT HAS A FIXED SIZE 
    MSG_SIZE,
    /// MESSAGE THAT IS SENT FROM THE CLIENT TO THE SERVER RIGHT AFTER CONNECTING TO REQUEST RENDERING WORK
    CLNT_REQUEST,
    /// MESSAGE THAT IS SENT FROM THE CLIENT TO THE SERVER AFTER INITIALIZING ITS GRAPHICS ENGINE
    CLNT_ENGINE_READY,
    /// MESSAGE THAT IS SENT FROM THE CLIENT TO THE SERVER AND CONTAINS THE GEOMETRY PASS RESULT
    CLNT_GEOMETRY_PASS,
    /// MESSAGE THAT IS SENT FROM THE CLIENT TO THE SERVER AND CONTAINS THE LIGHT PASS TEXTURES
    CLNT_LIGHT_PASS,
    /// MESSAGE THAT IS SENT FROM THE SERVER TO THE CLIENTS TO SETUP THEIR PIPELINE ( CLIENT ID, RENDER SIZE - VIEWPORT OPTIONS ETC)
    SRV_SETUP,
    /// MESSAGE THAT IS SENT FROM THE SERVER TO THE CLIENTS TO UPDATE THEIR SCENE ( LOAD + UNLOAD + TRANSFORMATIONS OF OBJECTS) - NOT LIGHTS
    SRV_SCENE_UPDATE,
    /// MESSAGE THAT IS SENT FROM THE SERVER TO THE CLIENTS THAT INCLUDE THE GEOMETRY PASS TEXTURE + LIGHT OBJECTS INFORMATION
    SRV_GEOMETRY_PASS_PLUS_LIGHTS
  };


  /// enums for object trnaformation
  /// can be used for lights as well
  enum class ObjectTransformType : char
  {
    OBJ_POS,    ///< requires object ID, new obsolute position
    OBJ_ROT,    ///< requires object ID, new obsolute orientation
    OBJ_SCALE,  ///< requires object ID, new absolute scale ( not applicable for lighs)
    LGHT_AMBIENT,  /// ONLY FOR LIGHTS (this one only for directional light)
    LGHT_DIFFUSE,  /// ONLY FOR LIGHTS
    LGHT_SPECULAR,  /// ONLY FOR LIGHTS
    LGHT_ATTENUATION,  /// ONLY FOR LIGHTS - 3 attenuation values
  };

  enum class ObjectType : char
  {
    CUBE,
    SKYBOX,
    SPHERE,
    MESH
  };

  struct ObjAddInfo
  {
    uint32_t m_id;
    ObjectType m_objType;
    RenderControl::GeometryPassMaterialFlags m_materialFlags;
    RenderControl::LightTypeFlags m_lightFlags;
  };

  struct ObjTransformInfo
  {
    uint32_t m_id;
    ObjectTransformType m_transformType;
    float x;
    float y;
    float z;
  };


  struct TextureChangeInfo
  {
    uint32_t m_id;
    uint32_t m_textureLayer;
    std::string m_path;
  };



  class NetworkMsg
  {
    char* m_data;   /// the actual data that are being transfered
    uint32_t m_size;    /// the size of the message
    uint32_t m_trueSize;    /// the size of the character array
    MsgType m_type;

  public:
    NetworkMsg();
    ~NetworkMsg();

    inline char* GetData() const{  return m_data;  }
    inline MsgType GetType() const{  return m_type;  }
    inline uint32_t GetSize() const{  return m_size; }
    
    
    // create functions
    void CreateSizeMsg(const uint32_t& a_size); ///< MSG_SIZE
    void CreateClientRequestMsg(); ///< CLNT_REQUEST
    void CreateEngineReadyMsg(const uint32_t& a_clientId); ///< CLNT_ENGINE_READY
    void CreateGeometryPassMsg(const uint32_t& a_clientId, char* a_textureData, const uint32_t& a_textureDataSize);  ///< CLNT_GEOMETRY_PASS
    void CreateLightPassMsg(const uint32_t& a_clientId, char* a_textureData, const uint32_t& a_textureDataSize);  ///< CLNT_LIGHT_PASS
    
    void CreateSetupMsg(const uint32_t& a_clientId, const glm::vec2& a_geometryPassTexSize, const glm::vec4& a_viewportInfo, const glm::vec2& a_lightPassTexSize ); ///< SRV_SETUP
    void CreateSceneUpdateMsg(const std::vector<ObjAddInfo>& a_objsToAdd, const std::vector<uint32_t>& a_objsToRemove, const std::vector<ObjTransformInfo>& a_objsToTransform, const std::vector<TextureChangeInfo>& a_textureChange);  ///< SRV_SCENE_UPDATE
    void CreateGeometryPassPlusLightsMsg(char* a_textureData, const uint32_t& a_textureDataSize, const std::vector<ObjAddInfo>& a_lightsToAdd, const std::vector<uint32_t>& a_lightsToRemove, const std::vector<ObjTransformInfo>& a_lightsToTransform );  ///< SRV_GEOMETRY_PASS_PLUS_LIGHTS
    
    
    // deserialize functions
    MsgType GetType(char* a_data, const uint32_t& a_dataSize) const;
    bool DeserializeSizeMsg(char* a_data, const uint32_t& a_dataSize, uint32_t& a_outSize) const; ///< MSG_SIZE
    // bool DeserializeClientRequestMsg(char* a_data, const uint32_t& a_dataSize, uint32_t& a_outSize) const; ///< CLNT_REQUEST
    bool DeserializeEngineReadyMsg(char* a_data, const uint32_t& a_dataSize, uint32_t& a_outClientId) const; ///< CLNT_ENGINE_READY
    bool DeserializeGeometryPassMsg(char* a_data, const uint32_t& a_dataSize, uint32_t& a_outClientId, char* a_outTextureData, uint32_t& a_outTextureDataSize) const; ///< CLNT_GEOMETRY_PASS
    bool DeserializeLightPassMsg(char* a_data, const uint32_t& a_dataSize, uint32_t& a_outClientId, char* a_outTextureData, uint32_t& a_outTextureDataSize) const; ///< CLNT_LIGHT_PASS
    
    bool DeserializeSetupMsg(char* a_data, const uint32_t& a_dataSize, uint32_t& a_outClientId, glm::vec2& a_outGeometryPassTexSize, glm::vec4& a_outViewportInfo, glm::vec2& a_outLightPassTexSize) const; ///< SRV_SETUP
    bool DeserializeSceneUpdateMsg(char* a_data, const uint32_t& a_dataSize, std::vector<ObjAddInfo>& a_outObjsToAdd, std::vector<uint32_t>& a_outObjsToRemove, std::vector<ObjTransformInfo>& a_outObjsToTransform, std::vector<TextureChangeInfo>& a_outTextureChange);  ///< SRV_SCENE_UPDATE
    bool DeserializeGeometryPassPlusLightsMsg(char* a_data, const uint32_t& a_dataSize, char* a_outTextureData, uint32_t& a_outTextureDataSize, std::vector<ObjAddInfo>& a_outLightsToAdd, std::vector<uint32_t>& a_outLightsToRemove, std::vector<ObjTransformInfo>& a_outLightsToTransform );  ///< SRV_GEOMETRY_PASS_PLUS_LIGHTS 
    
    /// reset the message creating an internal buffer of at least the given size
    /// the reasoning behind this function is that the internal buffer will be reallocated ONLY if it needs to grow (making a message object reusable and efficient)
    void Reset(const uint32_t& a_sizeTrue = 0); 
    void Clear(); /// frees the internal buffer
  };

}
