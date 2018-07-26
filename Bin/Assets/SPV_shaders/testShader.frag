#version 450


layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform sampler2D sampler0;

void main() {
  outColor = vec4(fragColor, 1.0);
  // outColor = texture(sampler0, vTexCoord);
}