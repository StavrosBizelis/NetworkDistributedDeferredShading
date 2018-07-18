#version 450 core

layout (std140, set = 0, binding = 0) uniform GlobalMatrices 
{
    mat4 projMatrix;
} globalsMats;

layout (std140, set = 0, binding = 1) uniform ObjectMatrices 
{
    mat4 modelViewMatrix;
    mat3 normalMatrix;
} objectMats;


// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;



layout(location = 0) out vec3 worldPosition;	// used for skybox


// This is the entry point into the vertex shader
void main()
{	

  worldPosition = inPosition;
  
	// Transform the vertex spatial position using 
	gl_Position = globalsMats.projMatrix * objectMats.modelViewMatrix * vec4(inPosition, 1.0f);

} 
	