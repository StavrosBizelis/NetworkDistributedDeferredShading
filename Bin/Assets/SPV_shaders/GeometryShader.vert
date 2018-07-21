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





// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;




/*out vertexOut
{

vec2 vTexCoord;	// Texture coordinate
vec3 vEyeNorm;	// normal coordinate
vec3 vEyeTan;	// tangent
vec3 vModelPosition;
}vo;
*/

layout (location = 0) out vec2 vTexCoord;	// Texture coordinate
layout (location = 1) out vec3 vEyeNorm;	// normal coordinate
layout (location = 2) out vec3 vEyeTan;	// tangent


// This is the entry point into the vertex shader
void main()
{	
  mat4 l_modelViewMat = globalsMats.viewMatrix * objectMats.modelMatrix;
	// Transform the vertex spatial position using 
	gl_Position = globalsMats.projMatrix * l_modelViewMat * vec4(inPosition, 1.0f);
	//vo.vModelPosition = ( matrices.modelMatrix * vec4(inPosition, 1.0f) ).xyz;
  
  
  
  vEyeTan = normalize(inverse(transpose(l_modelViewMat) * inTangent);
  vEyeNorm = normalize(inverse(transpose(l_modelViewMat) * inNormal);

	// Pass through the texture coordinate
	vTexCoord = inCoord;
} 
	