#version 450 core

layout (std140, set = 0, binding = 0) uniform GlobalMatrices 
{
  mat4 projMatrix;
  mat4 viewMatrix;
} globalsMats;

layout (std140, set = 0, binding = 1) uniform ObjectMatrices 
{
  mat4 modelMatrix;
} objectMats;


layout (location = 0) in vec3 inPosition;

layout(location = 0) out vec2 vTexCoord;


void main()
{          
  // Transform the vertex spatial position using 
  // gl_Position = matrices.projModelViewMatrixStack * vec4(inPosition, 1.0f);

  mat4 l_modelViewMat = globalsMats.viewMatrix * objectMats.modelMatrix;
	// Transform the vertex spatial position using 
	gl_Position = globalsMats.projMatrix * l_modelViewMat * vec4(inPosition, 1.0f);  
  
  
  gl_Position.y = -gl_Position.y;
  gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
  
}
