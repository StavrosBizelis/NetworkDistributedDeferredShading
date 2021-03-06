#version 400 core

uniform struct Matrices
{
  mat4 projModelViewMatrixStack;
} matrices;

layout (location = 0) in vec3 inPosition;

void main()
{          
  // Transform the vertex spatial position using 
  gl_Position = matrices.projModelViewMatrixStack * vec4(inPosition, 1.0f);
}
