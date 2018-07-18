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

	// Transform the vertex spatial position using 
	gl_Position = globalsMats.projMatrix * objectMats.modelViewMatrix * vec4(inPosition, 1.0f);
	//vo.vModelPosition = ( matrices.modelMatrix * vec4(inPosition, 1.0f) ).xyz;
  
    vEyeTan = normalize(objectMats.normalMatrix * inTangent);
    vEyeNorm = normalize(objectMats.normalMatrix * inNormal);

	// Pass through the texture coordinate
	vTexCoord = inCoord;
} 
	