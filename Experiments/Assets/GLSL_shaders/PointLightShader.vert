#version 400 core

uniform struct Matrices
{
  mat4 projModelViewMatrixStack;  
} matrices;

layout (location = 0) in vec3 inPosition;

uniform vec2 UScreenResolution = vec2( 1680, 1050);
out vec2 vTexCoord;


void main()
{          
  // Transform the vertex spatial position using 
  gl_Position = matrices.projModelViewMatrixStack * vec4(inPosition, 1.0f);
}
