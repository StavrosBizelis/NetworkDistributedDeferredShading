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

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;

layout(location = 0) out vec3 fragColor;

vec2 positions[4] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5),
    vec2(1, 1)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);



void main() {
  gl_Position = globalsMats.projMatrix * globalsMats.viewMatrix * objectMats.modelMatrix * vec4(inPosition.xy, 0.0, 1.0);
  // gl_Position =  vec4(positions[gl_VertexIndex],0.0, 1.0);
  
  gl_Position.y = -gl_Position.y;
  gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
  // works
  // fragColor = vec3( globalsMats.viewMatrix[0][0], globalsMats.viewMatrix[1][0], globalsMats.viewMatrix[2][2] );
  // works only with the first vertex
  fragColor = vec3( (inCoord.x), abs(inCoord.y), 0 );
}