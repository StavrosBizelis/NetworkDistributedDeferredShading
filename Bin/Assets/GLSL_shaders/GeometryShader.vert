#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
	mat4 modelMatrix;
} matrices;





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

out vec2 vTexCoord;	// Texture coordinate
out vec3 vEyeNorm;	// normal coordinate
out vec3 vEyeTan;	// tangent


// This is the entry point into the vertex shader
void main()
{	

	// Transform the vertex spatial position using 
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0f);
	//vo.vModelPosition = ( matrices.modelMatrix * vec4(inPosition, 1.0f) ).xyz;
  
    vEyeTan = normalize(matrices.normalMatrix * inTangent);
    vEyeNorm = normalize(matrices.normalMatrix * inNormal);

	// Pass through the texture coordinate
	vTexCoord = inCoord;
} 
	