#version 450 core
layout ( binding = 0) uniform GlobalMatrices 
{
    mat4 projMatrix;
    mat4 viewMatrix;
} globalsMats;

layout ( binding = 1) uniform ObjectMatrices 
{
    mat4 modelMatrix;
} objectMats;





// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;

layout (location = 0) out vec2 vTexCoord;


void main()
{
  
  mat4 l_modelViewMat = globalsMats.viewMatrix * objectMats.modelMatrix;
  gl_Position = globalsMats.projMatrix * l_modelViewMat * vec4(inPosition, 1.0f);
  
  gl_Position.y = -gl_Position.y;
  gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
// gl_Position.z = -10;
  
	vTexCoord = inCoord;
}