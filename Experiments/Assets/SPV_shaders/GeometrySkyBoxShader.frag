#version 450 core


layout(location = 0) in vec3 worldPosition;

layout (set = 0, binding = 2) uniform samplerCube UCubeMap;  // The diffuse colour sampler


layout(location = 0) out vec4 diffuseOut;
layout(location = 1) out vec4 normalOut;
layout(location = 2) out vec4 specularOut;
layout(location = 3) out vec4 emissiveOut;


void main()
{
  normalOut = vec4(vec3(0), 1); 	  
  diffuseOut = vec4(vec3(0), 1);
  specularOut = vec4(vec3(0), 1);
  emissiveOut = texture(UCubeMap, worldPosition);
}
