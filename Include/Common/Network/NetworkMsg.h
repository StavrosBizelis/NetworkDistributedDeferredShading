#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Common/RenderControl/ADeferredShadingPass.h"

#include <iostream>
#include <memory>
#include <array>
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
    /// MESSAGE THAT IS SENT FROM THE CLIENT TO THE SERVER AND CONTAINS THE RENDERING RESULT
    CLNT_RENDER_RESULT,
    /// MESSAGE THAT IS SENT FROM THE SERVER TO THE CLIENTS TO SETUP THEIR PIPELINE ( CLIENT ID, RENDER SIZE - VIEWPORT OPTIONS ETC)
    SRV_SETUP,
    /// MESSAGE THAT IS SENT FROM THE SERVER TO THE CLIENTS TO UPDATE THEIR SCENE ( LOAD + UNLOAD + TRANSFORMATIONS OF OBJECTS) + LIGHTS
    SRV_SCENE_UPDATE
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
    std::string m_meshPath;
    
    void Serialize(char* a_arrOut) const;
    void Deserialize(char* a_arrIn);
    size_t Size() const;
  };

  struct ObjTransformInfo
  {
    uint32_t m_id;
    ObjectTransformType m_transformType;
    float x;
    float y;
    float z;
    
    void Serialize(char* a_arrOut) const;
    void Deserialize(char* a_arrIn);
    size_t Size() const;
  };


  struct TextureChangeInfo
  {
    uint32_t m_id;
    uint32_t m_textureLayer;
    char m_cubeText;
    std::string m_path[6];
  };


  class NetworkMsg;
  typedef std::shared_ptr<NetworkMsg> NetworkMsgPtr;
  class NetworkMsg
  {
    char* m_data;   /// the actual data that are being transfered
    uint32_t m_size;    /// the size of the message
    uint32_t m_trueSize;    /// the size of the character array
    MsgType m_type;

  public:
    NetworkMsg();
    ~NetworkMsg();

        
    NetworkMsg(const NetworkMsg& a_other); ///< Copy constructor
    NetworkMsg(NetworkMsg&& a_other); ///< Move constructor
    NetworkMsg& operator=(const NetworkMsg& a_other); ///< Copy assignment operator
    NetworkMsg& operator=(NetworkMsg&& a_other);  ///< Move assignment operator

    
    inline char* GetData(){  return m_data;  }
    inline const char* GetData() const{  return m_data;  }
    MsgType GetType() const;
    
    inline uint32_t GetSize() const{  return m_size; }
    inline uint32_t& GetSize() {  return m_size; }
    inline void SetSize(const uint32_t& a_size) {  Reset(a_size); m_size = a_size; }
    
    void SetData(char* a_data, const uint32_t& a_size)
    {
      Clear();
      m_data = a_data; 
      m_trueSize = a_size;
      m_size = a_size;
    }
    
    // create functions
    void CreateSizeMsg(const uint32_t& a_size); ///< MSG_SIZE
    void CreateClientRequestMsg(); ///< CLNT_REQUEST
    void CreateEngineReadyMsg(); ///< CLNT_ENGINE_READY
    void CreateRenderResultMsg(char* a_textureData, const uint32_t& a_textureDataSize, const glm::vec2& a_resolution);  ///< CLNT_RENDER_RESULT
    
    void CreateSetupMsg( const glm::vec4& a_viewportInfo, const glm::vec2& a_partialResolution, const glm::vec2& a_resolution ); ///< SRV_SETUP
    void CreateSceneUpdateMsg(
      const std::array<glm::vec3, 3>& a_cameraSettings,  /// pos, view, up vec
      const std::vector<ObjAddInfo>& a_objsToAdd, 
      const std::vector<uint32_t>& a_objsToRemove, 
      const std::vector<ObjTransformInfo>& a_objsToTransform, 
      const std::vector<TextureChangeInfo>& a_textureChange,
      
      const std::vector<ObjAddInfo>& a_lightsToAdd,
      const std::vector<uint32_t>& a_lightsToRemove,
      const std::vector<ObjTransformInfo>& a_lightsToTransform
      );  ///< SRV_SCENE_UPDATE
    
    
    // deserialize functions
    bool DeserializeSizeMsg( uint32_t& a_outSize) const; ///< MSG_SIZE
    // bool DeserializeClientRequestMsg( uint32_t& a_outSize) const; ///< CLNT_REQUEST
    // bool DeserializeEngineReadyMsg() const; ///< CLNT_ENGINE_READY
    
    /// @brief a_outTextureData should be allocated beforehand and is independent to the network message
    /// @brief if a_outTextureData is nullptr - only a_outTextureDataSize will be updated
    /// @brief the caller is responsible to delete a_outTextureData after this function is called
    bool DeserializeRenderResultMsg( char* a_outTextureData, uint32_t& a_outTextureDataSize, glm::vec2& a_outResolution) const; ///< CLNT_RENDER_RESULT
    /// @brief a_outTextureData should not be allocated beforehand and is bound to the network message
    /// @brief do not try to delete a_outTextureData after calling this function
    bool DeserializeRenderResultMsgWithoutCopy( char*& a_outTextureData, uint32_t& a_outTextureDataSize, glm::vec2& a_outResolution) const; ///< CLNT_RENDER_RESULT
    
    bool DeserializeSetupMsg( glm::vec4& a_outViewportInfo, glm::vec2& a_outPartialResolution, glm::vec2& a_outResolution ) const; ///< SRV_SETUP
    bool DeserializeSceneUpdateMsg( 
      std::array<glm::vec3, 3>& a_outCameraSettings,
      std::vector<ObjAddInfo>& a_outObjsToAdd, 
      std::vector<uint32_t>& a_outObjsToRemove, 
      std::vector<ObjTransformInfo>& a_outObjsToTransform, 
      std::vector<TextureChangeInfo>& a_outTextureChange,
      
      std::vector<ObjAddInfo>& a_outLightsToAdd, 
      std::vector<uint32_t>& a_outLightsToRemove, 
      std::vector<ObjTransformInfo>& a_outLightsToTransform
      ) const ;  ///< SRV_SCENE_UPDATE
    
    
    /// reset the message creating an internal buffer of at least the given size
    /// the reasoning behind this function is that the internal buffer will be reallocated ONLY if it needs to grow (making a message object reusable and efficient)
    void Reset(const uint32_t& a_sizeTrue = 0); 
    void Clear(); /// frees the internal buffer
    
    friend std::ostream& operator<<(std::ostream &strm, const NetworkMsg &a);
  };
  
  std::ostream& operator<<(std::ostream &strm, const NetworkMsg &a);
  std::ostream& operator<<(std::ostream &strm, const ObjAddInfo &a);

}
